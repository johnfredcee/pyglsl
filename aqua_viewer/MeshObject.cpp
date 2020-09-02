#include "StdAfx.h"

#include <aqua/VertexBufferData.h>
#include <aqua/VertexIndexBufferData.h>
#include <aqua/SkeletonBufferData.h>
#include <aqua/Material.h>
#include <aqua/AnimationData.h>
#include <GraphicsSystem/GraphicsSystem.h>
//#include <GraphicsSystem/ShaderSupport.h>

#include <FileIO/TextFile.h>
#include <CRC/CRC.h>
#include <aqua/Material.h>

// this is last because Playbox uses GLee, which wants to be the first
// to pull in Opengl, but we need glcanvas for wxGLApp
#include <wx/glcanvas.h>


#include "Rtti.h"
#include "Config.h"
#include "IntermediateMesh.h"
#include "IMesh.h"
#include "FbxParse.h"
#include "MetaMaterialManager.h"
#include "MetaBitmapManager.h"
#include "MetaBufferManager.h"
#include "TextureDefinitionManager.h"
#include "ShaderNameManager.h"
#include "Viewer.h"
#include "IGraphicObject.h"
#include "SceneManager.h"
#include "GraphicObject.h"
#include "GraphicObjectNameManager.h"
#include "AnimationTimeStamp.h"
#include "AnimationTranslationKey.h"
#include "AnimationRotationKey.h"
#include "AnimationKey.h"
#include "AnimationTrack.h"
#include "AnimationContainer.h"
#include "Skeleton.h"
#include "MeshObject.h"


using namespace aqua;

template<> void MetaSerialiser<SkeletonMesh>(const SkeletonMesh& data, DynamicMetaParser& parser);

   
MeshObject::MeshObject(FbxParse* parser, FbxNode* pNode, shared_ptr<DynamicMetaData> metaData) {
	// common stuff
	initialise(pNode);
	// work out what layer we are in
	FbxDisplayLayer *layer = parser->getDisplayLayer(pNode);
	std::string nodeName(pNode->GetName());
	wxString displayLayerName("static_furniture");
	if (layer != NULL)
		displayLayerName = layer->GetName();
	wxLogVerbose("Mesh Node %s In Display Layer %s ", wxString(pNode->GetName(), wxConvUTF8).c_str(), displayLayerName.utf8_str());
	// NOTE: It is assumed by this time that the mesh has been triangluated.
	FbxMesh *pMesh = FbxCast<FbxMesh>(pNode->GetNodeAttribute());
	pMesh->ComputeVertexNormals();
	wxLogVerbose("Mesh has %d layers ", pMesh->GetLayerCount());
	int polygonCount = pMesh->GetPolygonCount();
	// vector of materials actually used
	std::vector<int>			usedMaterials;
	// vector to map index of actually used material to source FBX material
	std::vector<int>			sourceMaterialsIndex(polygonCount);
	// uvsets used by this material
	std::vector<std::string>	uvSetNames;
	// work out which materials and uvSets are actually used
	parseLayers(pNode, pMesh, usedMaterials, sourceMaterialsIndex, uvSetNames);
	// ok, lets build an IntermediateMesh -- just vertices and polys for now
	aqua::shared_ptr<OpenMesh::IntermediateMesh> iMesh = IMesh::makeIntermediateMesh(pMesh);	
	// now, test for the presence of a skeleton
	aqua::shared_ptr<SkeletonMesh> skeletonMesh;
	if (SkeletonMesh::hasSkeleton(pMesh)) {
		wxLogDebug("Found skeleton!");
		skeletonMesh = SkeletonMesh::extractSkeleton(pMesh);
		if (skeletonMesh != NULL) {
			skeletonMesh->extractAnimation(parser, pMesh);
		}
		else {
			wxLogDebug("Failed to Extract skeleton.");
		}
		IMesh::addSkeletonAttributes(iMesh);
	} 

	// ensure skeleton gets added to intermediate mesh
	
	// array of materials associated with mesh
	std::vector< aqua::shared_ptr< Material > > materials;
	// array of ids of materials associated with mesh
	std::vector< crc32 >						materialIds;
	// map of associated textures with each material (indexed by materialId)
	IMesh::MaterialTextureDictionary			materialTextureInfo;
	// checkout services we will need
	const aqua::Locator::Service<MetaMaterialManager>&		metaMaterialManager(aqua::Locator::checkOut<MetaMaterialManager>());
	const aqua::Locator::Service<ShaderNameManager>&		shaderManager(aqua::Locator::checkOut<::ShaderNameManager>());
	// build up the materials
	for (Uint32 materialIndex = 0U; materialIndex < usedMaterials.size(); ++materialIndex) {
		// locate the fbx material
		int fbxMaterialIndex			 = usedMaterials[materialIndex];
		FbxSurfaceMaterial *fbxMaterial = (FbxSurfaceMaterial *)pNode->GetSrcObject(FbxSurfaceMaterial::ClassId, fbxMaterialIndex);
		if (0 == fbxMaterial)
			continue;
		wxLogVerbose(" -- Parsing material %d (fbx % d) ", materialIndex, fbxMaterialIndex);
		wxLogVerbose(wxString::Format("Surface Material %d : %s", materialIndex, fbxMaterial->GetName()));
		// set up the aqua info for this material
		aqua::shared_ptr<Material>		  material(new(Material));
		// material portion of material name
		std::string						  materialName;
		// shader portion of material name
		std::string						  shaderName;
		// true if *any* of the textures has alpha
		//bool							  materialHasAlpha	= false;
		// texture associated with material
		std::vector<IMesh::TextureInfo>	  textureInfo;			
		// parse an fbx material into a aqua::Material
		parseFbxNodeMaterial(material, pNode, pMesh, fbxMaterialIndex, materialName, shaderName);
		// ensure we had a valid shader in the material name
		if (!shaderManager->validateShaderName(shaderName)) {
			const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
			sceneManager->markAsUnexportable();
			aqua::Locator::checkIn(sceneManager);
			wxLogError("Material %s : %s does not look like a valid shader name.", materialName.c_str(), shaderName.c_str());
		}
		//go through all the possible textures to fill in material texture slots
		if (fbxMaterial) {
			// if we are using textures
			if (!uvSetNames.empty())	 {
				// we only worry about textures in the diffuse colour channel for now - lets get em
				wxLogVerbose(wxT(" -- Parsing textures for material %s %d (%d) "), materialName.c_str(), materialIndex, fbxMaterialIndex);
				getTextureInformation(pNode, fbxMaterial, textureInfo);
				wxLogVerbose(wxT(" %d textures "), textureInfo.size());
				// fill in the texture slots
				int textureSlotIndex = 0;
				for(std::vector<IMesh::TextureInfo>::iterator ti_it = textureInfo.begin(); ti_it != textureInfo.end(); ++ti_it) {
					// create an image for the texture - and check it meets the obvious requirements.
					wxImage textureImage;
					bool textureHasAlpha;
					validateTexture(pNode, textureImage, textureHasAlpha, ti_it);
					TextureBlendMode blendMode = ti_it->blendMode;
					// create bitmap
					BitmapData bitmap;
					bitmap.width = textureImage.GetWidth();
					bitmap.height = textureImage.GetHeight();
					bitmap.attributes = NO_ATTRIBUTES;
					bitmap.pixelFormat = textureImage.HasAlpha() ? RGBA32 : RGB24;
					// this sets the size and returns a pointer
					// note that BinBlock destructor frees this, so we don't use a smart ptr					
					Byte* data(textureData(textureImage, bitmap.data.sizeInBytes));
					bitmap.data.dataPtr = data;
					// add bitmap to metadata
					const aqua::Locator::Service<MetaBitmapManager> &bitmapManager(aqua::Locator::checkOut<MetaBitmapManager>());
					wxString intermediateBitmapName(ti_it->fileName.GetFullName());
					intermediateBitmapName.Replace(wxT(" "), wxT("_"));
					std::string bitmapName(intermediateBitmapName.utf8_str());
					std::string originalName(ti_it->fileName.GetFullPath(wxPATH_UNIX).utf8_str());
					crc32 newBitmapId = bitmapManager->addBitmap(bitmapName, bitmap);
					int attributes = (int)shaderManager->getTextureSlotFlag(shaderName, textureSlotIndex);
					// add it to the texture defintion manager so it can be output to the texture specification tool
					// TODO : This is probably redundant now that textures are metadata -- but needs toolchain work
					const aqua::Locator::Service<TextureDefinitionManager> &textureDefinitionManager(aqua::Locator::checkOut<TextureDefinitionManager>()); 
					textureDefinitionManager->addTexture(newBitmapId,
														 bitmapName,
														 textureImage.GetWidth(), textureImage.GetHeight(),
														 originalName,
														 textureHasAlpha,
														 attributes);
					aqua::Locator::checkIn(textureDefinitionManager);
					// work out which slot it should go into based on uv map index
					int uvMapIndex = std::find(uvSetNames.begin(), uvSetNames.end(), ti_it->uvSetName) - uvSetNames.begin();
					ti_it->uvSetIndex = (uvMapIndex < (int) uvSetNames.size()) ? uvMapIndex : 0;
					PBASSERT((ti_it->uvSetIndex < (int) uvSetNames.size()), "Missing UV Map");
					// check we don't have something bound to a uv set we don't know about (actually possible if artists use > 2 maps)
					if(uvMapIndex >= (int) uvSetNames.size())
						wxLogError(wxT("Missing UV map or bitmap on %s? Are non - bitmap materials being used?"),  fbxMaterial->GetName());
					// use the texture defintion added above to validate that the texture meets the requirement of the shader
					if(!shaderManager->validateTextureForShader(shaderName, newBitmapId, textureSlotIndex)) {
						wxLogError(wxString::Format(wxT("Object %s uses material %s with shader %s but texture alpha is not set up for it."),
													pNode->GetName(),
													materialName.c_str(),
													shaderName.c_str()));
						wxLogError(wxString::Format(wxT("%s is not	an appropiate texture"), ti_it->originalName.GetFullPath()));
						const aqua::Locator::Service<SceneManager> &sceneManager(aqua::Locator::checkOut<SceneManager>()); 
						sceneManager->markAsUnexportable();
						aqua::Locator::checkIn(sceneManager);
					}
					// add the texture to the material
					TextureSlotData textureData;
					textureData.texture					= aqua::HashString(bitmapName.c_str());
					textureData.textureCoordSlot		= uvMapIndex;
					textureData.blendMode				= blendMode;
					textureData.specialMode				= SPECIAL_NONE;
					textureData.uModPerSecond			= 0.0f;
					textureData.vModPerSecond			= 0.0f;
					textureData.textureAnimationEnabled = false;
					material->addTexture(textureData);
					textures_.push_back(newBitmapId);
					aqua::Locator::checkIn(bitmapManager);												
					++textureSlotIndex;						
				} // for each texture
			} 
			else {
				// no uv maps on material oops
				wxLogError("On Object %s ", wxString(pNode->GetName(), wxConvUTF8).c_str());
				wxLogError(" -- Not parsing textures for material %s %d (%d) since no uvSets associated with it. ", materialName.c_str(), materialIndex, fbxMaterialIndex);
				const aqua::Locator::Service<SceneManager> &sceneManager(aqua::Locator::checkOut<SceneManager>()); 
				sceneManager->markAsUnexportable();
				aqua::Locator::checkIn(sceneManager);					
			} // if !uvSetNames.empty()
		} else {
			// no material!
			wxLogError("Object w/o material. Should NOT be possible!!!");
			const aqua::Locator::Service<SceneManager> &sceneManager(aqua::Locator::checkOut<SceneManager>()); 
			sceneManager->markAsUnexportable();
			aqua::Locator::checkIn(sceneManager);					
		} // if fbxMaterial

		// validate the material identity!
		if (metaMaterialManager->checkName(materialName)) {
			if (!metaMaterialManager->checkMaterial(materialName, material)) {
				wxLogError("Duplicate Material: %s appears to be modified (duplicate name, different material) on Object %s.", materialName.c_str(),  pNode->GetName());
				const aqua::Locator::Service<SceneManager> &sceneManager(aqua::Locator::checkOut<SceneManager>()); 
				sceneManager->markAsUnexportable();
				aqua::Locator::checkIn(sceneManager);									
			}			
		} else {
			// if it didn't exist, then it goes in!
			metaMaterialManager->addMaterial(materialName, *material);
		}			
		// add the material to the list of materials
		materials.push_back(material);
		materialIds.push_back(CalcCRC(materialName.c_str()));
		materials_.push_back(CalcCRC(materialName.c_str()));
		materialTextureInfo[materialIndex] = textureInfo;						
	} // for each material

	// create and fill the intermediate mesh we are going to create our vertex buffers from!
	IMesh::addIntermediateMeshAttributes(iMesh);
	IMesh::fillIntermediateMeshAttributes(iMesh, pNode, pMesh, sourceMaterialsIndex, materialTextureInfo);
	IMesh::calcBoundingBox(iMesh, minExtent_, maxExtent_);

	// skeleton and animation handling
	const aqua::Locator::Service<MetaBufferManager> &metaBufferManager(aqua::Locator::checkOut<MetaBufferManager>());	
	std::string underscore("_");	
	// allocate a skeleton if there is one
	if (skeletonMesh) {
		IMesh::fillSkeletonAttributes(iMesh, pNode, pMesh);
		// TODO -- isn't this premature?
		const aqua::Locator::Service<GraphicObjectNameManager> &graphicObjectNameManager(aqua::Locator::checkOut<GraphicObjectNameManager>()); 
		const aqua::Locator::Service<SceneManager> &sceneManager(aqua::Locator::checkOut<SceneManager>()); 
		std::string skeletonName(graphicObjectNameManager->stripInstance(nodeName) + underscore + std::string("skeleton"));
		std::string skeletonDataName("SkeletonData");
		DynamicMetaParser skeletonParser(sceneManager->allocateData(skeletonName, skeletonDataName));
		MetaSerialiser<SkeletonMesh>(*skeletonMesh, skeletonParser);
		// if there are animations, extract those, too
		boost::shared_ptr<const AnimationClipContainer> animations(skeletonMesh->getAnimations());
		if (animations) {
			MetaSerialiser<AnimationClipContainer>(*animations,skeletonParser);
		}
		aqua::Locator::checkIn(sceneManager);		
		aqua::Locator::checkIn(graphicObjectNameManager);		
	}
	
	// scan each material and create a vertex buffer for each
	for (Uint32 materialIndex = 0U; materialIndex < usedMaterials.size(); ++materialIndex) {
		// create a name for the buffers
		const aqua::Locator::Service<GraphicObjectNameManager> &graphicObjectNameManager(aqua::Locator::checkOut<GraphicObjectNameManager>()); 
		std::string bufferName(graphicObjectNameManager->stripInstance(nodeName)  +	 underscore + metaMaterialManager->getName(materialIds[materialIndex]));
		//crc32 bufferCrc = graphicObjectNameManager->getInstanceCRC(bufferName);
		if (!graphicObjectNameManager->instanceSeen(bufferName)) {
			Array<IndexData> indices;
			Array<BoneInfluence> skeletonVerts;
			bool instancedIndices = false;
			bool instancedVertices = false;
			shared_ptr<VertexBufferData> vertices(metaBufferManager->fillVertexBuffer(bufferName, instancedVertices, indices, skeletonVerts, instancedIndices, iMesh ,materialIndex));
			metaBufferManager->addBuffers(bufferName, instancedVertices, vertices, instancedIndices, indices, skeletonVerts);
			Uint32 format = vertices->getFormat();			
			// validate that the buffers are in the correct format
			std::string shaderName(metaMaterialManager->getShaderName(materialIds[materialIndex]));
			if (!shaderManager->validateShaderFormats(shaderName, format)) {
				std::string materialName(metaMaterialManager->getName(materialIds[materialIndex]));
				//wxMessageBox(wxString::Format("Object %s uses material %s with shader %s but does not seem to be correctly set up for it",
				//								pNode->GetName(),
				//								materialName.c_str(),
				//								shaderName.c_str()),
				//								pNode->GetName(),
				//								wxICON_ERROR | wxOK);
				wxLogError(wxString::Format("Object %s uses material %s with shader %s but does not seem to be correctly set up for it",
											pNode->GetName(),
											materialName.c_str(),
											shaderName.c_str()));				
				const aqua::Locator::Service<SceneManager> &sceneManager(aqua::Locator::checkOut<SceneManager>()); 
				sceneManager->markAsUnexportable();
				aqua::Locator::checkIn(sceneManager);
			}
		} 
		vertexBuffers_.push_back(vbufferId(bufferName));
		indexBuffers_.push_back(ibufferId(bufferName));						
		cyan::Locator::checkIn(graphicObjectNameManager);
	} // for  each material
	
	aqua::Locator::checkIn(metaBufferManager);
	aqua::Locator::checkIn(shaderManager);
	aqua::Locator::checkIn(metaMaterialManager);
}



//! initialise a component based on the data of this object
bool MeshObject::initialiseComponent(DynamicMetaData& metaData,	 const std::string& componentName, aqua::Uint32 componentTypeId) const
{
	DynamicMetaParser componentParser(metaData, CalcCRC(componentName.c_str()));
	PBASSERT(componentParser.IsValid(), "Unable to find component");
	if(componentTypeId == CalcCRC("GraphicModelComponentData")) {
		const aqua::Locator::Service<GraphicObjectNameManager> &graphicObjectNameManager(aqua::Locator::checkOut<GraphicObjectNameManager>()); 
		std::string objectName(getObjectName());
		std::string objectHint(getHint());
		// okeydokey, lets export it
		//Uint32 instancedObjectCRC = graphicObjectNameManager->getInstanceCRC(objectName);
		std::string instancedObjectName(graphicObjectNameManager->stripInstance(objectName));
		DynamicMetaParser groupNameParser(componentParser,	CalcCRC("groupName"));
		PBASSERT(componentParser.IsValid(), "Cant find group name field in GraphicModelComponentData");
		groupNameParser.SetValue(CalcCRC("DefaultGroupZeroQ"));
		// size the buffer arrays
		DynamicMetaParser vbufferParser(componentParser,   CalcCRC("vBuffer"));
		PBASSERT(vbufferParser.IsValid(), "Cant find vbuffer field in GraphicModelComponentData");
		vbufferParser.SetArrayCount(vertexBuffers_.size());
		DynamicMetaParser ibufferParser(componentParser,  CalcCRC("iBuffer"));
		PBASSERT(ibufferParser.IsValid(), "Cant find ibuffer field in GraphicModelComponentData");
		ibufferParser.SetArrayCount(indexBuffers_.size());
		DynamicMetaParser materialIdParser(componentParser, CalcCRC("materialId"));
		materialIdParser.SetArrayCount(materials_.size());
		// fill each buffer
		for(aqua::Uint32 vbCount = 0; vbCount < vertexBuffers_.size(); ++vbCount) {
			crc32 id = materials_[vbCount];
			const aqua::Locator::Service<MetaMaterialManager> &mnommanager(aqua::Locator::checkOut<MetaMaterialManager>()); 
    		std::string materialName = mnommanager->getName(id);
			// vbuffer name
			std::string name(wxString::Format("%s_%s_vertices", instancedObjectName.c_str(), materialName.c_str()).utf8_str());
			vbufferParser.Find(vbCount, MetaParser::FIND_BY_INDEX).SetValue(CalcCRC(name.c_str()));
			// index name
			name = std::string(wxString::Format("%s_%s_indices", instancedObjectName.c_str(), materialName.c_str()).utf8_str());
			ibufferParser.Find(vbCount, MetaParser::FIND_BY_INDEX).SetValue(CalcCRC(name.c_str()));
			// material name
			materialIdParser.Find(vbCount, MetaParser::FIND_BY_INDEX).SetValue(CalcCRC(materialName.c_str()));
			aqua::Locator::checkIn(mnommanager);
		}
		DynamicMetaParser bboxParser(componentParser, CalcCRC("boundingBox"));
		DynamicMetaParser minExtentParser(bboxParser, CalcCRC("minExtent"));
		minExtentParser.Find(CalcCRC("x")).SetValue(minExtent_.x());
		minExtentParser.Find(CalcCRC("y")).SetValue(minExtent_.y());
		minExtentParser.Find(CalcCRC("z")).SetValue(minExtent_.z());
		DynamicMetaParser maxExtentParser(bboxParser, CalcCRC("maxExtent"));
		maxExtentParser.Find(CalcCRC("x")).SetValue(maxExtent_.x());
		maxExtentParser.Find(CalcCRC("y")).SetValue(maxExtent_.y());
		maxExtentParser.Find(CalcCRC("z")).SetValue(maxExtent_.z());
		componentParser.GetDynamicMetaData()->AddHint(componentParser.GetId(), objectHint.c_str());
		aqua::Locator::checkIn(graphicObjectNameManager);
	}
	return GraphicObject::initialiseComponent(metaData, componentName, componentTypeId);
}

/**
 * Return ids of all metadata associated with this object
 * @param ids array of its to fill
 * @return true if ids is non empty
 */
bool MeshObject::ids(std::vector<crc32>& ids) {
	ids.insert(ids.begin(), vertexBuffers_.begin(), vertexBuffers_.end());
	ids.insert(ids.begin(), indexBuffers_.begin(), indexBuffers_.end());
	ids.insert(ids.begin(), materials_.begin(), materials_.end());
	ids.insert(ids.begin(), textures_.begin(), textures_.end());
	return !ids.empty();
}

Rtti MeshObject::type() const {
	return Rtti("MeshObject");
};

// need to use this so we can pass shared ptrs into the renderer
// which have been hoisted from inside meta data - we don't own them
struct null_deleter
{
	void operator()(void const *) const
	{
	}
};



/** Render a mesh object. */
void MeshObject::render() const {
	
	const aqua::Locator::Service<aqua::GraphicsSystem> &graphicsSystem(aqua::Locator::checkOut<aqua::GraphicsSystem>());
	const aqua::Locator::Service<SceneManager> &sceneManager(aqua::Locator::checkOut<SceneManager>()); 
	null_deleter i_dont_own_it;
	
	std::vector<crc32>::const_iterator vb_it;
	std::vector<crc32>::const_iterator vib_it = indexBuffers_.begin();
	std::vector<crc32>::const_iterator m_it	  = materials_.begin(); 
	for(vb_it = vertexBuffers_.begin(); vb_it != vertexBuffers_.end();
		++vb_it) {
		PBASSERT(m_it != materials_.end(), "ran off end of materials");
		PBASSERT(vib_it != indexBuffers_.end(), "ran off end of index buffers");
		DynamicMetaParser materialParser(sceneManager->fetchData(*m_it));
		PBASSERT(materialParser.IsValid(), "Couldn't find material");
		DynamicMetaParser indexBufferParser(sceneManager->fetchData(*vib_it));
		PBASSERT(indexBufferParser.IsValid(), "Couldn't find index buffer");		
		DynamicMetaParser bufferParser(sceneManager->fetchData(*vb_it));
		PBASSERT(bufferParser.IsValid(), "Couldn't find vertex buffer");
		aqua::shared_ptr<const Material> mat(reinterpret_cast<const Material*>(materialParser.GetData()), i_dont_own_it);
		aqua::shared_ptr<const VertexBufferData> vb(reinterpret_cast<const VertexBufferData*>(bufferParser.GetData()), i_dont_own_it);
		aqua::shared_ptr<const VertexIndexBufferData> vib(reinterpret_cast<const VertexIndexBufferData*>(indexBufferParser.GetData()), i_dont_own_it);
		graphicsSystem->drawStaticItem(mat, vb, vib,  transformMatrix_);
		++vib_it;
		++m_it;
	}
	aqua::Locator::checkIn(sceneManager);
	aqua::Locator::checkIn(graphicsSystem);	
}

/**
 * Strip the pathname off the front of the texture path
 * @param textureFilenme filename to process
 * @return everything after "art" in the filename
 */
std::string stripTextureFileName(const std::string& textureFilenme)
{
	wxFileName tempName(textureFilenme.c_str());
	wxArrayString texturePathComponents(tempName.GetDirs());
	wxFileName textureIdFileName(wxT(""), wxT(""), tempName.GetName(), wxT(""));
	bool foundArt = false;
	for(int i = texturePathComponents.GetCount() - 1; i>= 0; --i) {
		// add all dirs after "art"
		textureIdFileName.PrependDir(texturePathComponents[i]);
		if(texturePathComponents[i].CmpNoCase(wxT("art")) == 0) {
			foundArt = true;
			break;
		}
	}
	if (!foundArt)
		wxLogError("Texture %s does not live under an art/ folder.\n The texture converter will not process it.\n", tempName.GetFullPath());
			
	// ensure extenstion goes in as well
	textureIdFileName.SetExt(tempName.GetExt());
	wxLogVerbose("Texture Filename for CRC %s ", textureIdFileName.GetFullPath());
	return std::string(textureIdFileName.GetFullPath().c_str());
}
	
/**
 * Extract the texture information from a material and fill in the
 * materialTextureInfo vector
 * @param pNode FbxNode associated with texture
 * @param fbxMaterial Material that uses texture
 * @param materialTextureInfo Structure to fill with info
 * @return true if successful
 */
bool getTextureInformation(FbxNode *pNode, FbxSurfaceMaterial *fbxMaterial, std::vector<IMesh::TextureInfo>& materialTextureInfo) {
	bool result = true;
	FbxProperty lProperty;
	char* lBlendModes[]	  = { "Translucent", "Add", "Modulate", "Modulate2", "Over" };
	int lTextureIndex = 0;
	// only interested in diffuse at the moment
	//FOR_EACH_TEXTURE(lTextureIndex)
	{
		lProperty = fbxMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[lTextureIndex]);
		if (lProperty.IsValid()) {
			//Here we have to check if it's layeredtextures, or just textures:
			int lLayeredTextureCount = lProperty.GetSrcObjectCount(FbxLayeredTexture::ClassId);
			if (lLayeredTextureCount > 0) {
				// iterate through the layers
				wxLogVerbose(wxString::Format(" %d Texture Layers", lLayeredTextureCount));
				for (int j=0; j<lLayeredTextureCount; ++j) {
					//wxLogVerbose("	Layered Texture: %d ", j);
					FbxLayeredTexture *lLayeredTexture = FbxCast <FbxLayeredTexture>(lProperty.GetSrcObject(FbxLayeredTexture::ClassId, j));
					int lNbTextures = lLayeredTexture->GetSrcObjectCount(FbxTexture::ClassId);
					for (int k =0; k<lNbTextures; ++k) {
						IMesh::TextureInfo textureInfo;
						FbxTexture* lTexture = FbxCast <FbxTexture> (lLayeredTexture->GetSrcObject(FbxTexture::ClassId,k));
						if (lTexture) {
							FbxFileTexture* fbxTextureFile(FbxCast<FbxFileTexture>(lTexture));
							FbxLayeredTexture::EBlendMode lBlendMode;
							lLayeredTexture->GetTextureBlendMode(k, lBlendMode);
							//			wxLogVerbose(wxString::Format("	 Textures for %s ",	 wxString(lProperty.GetName(), wxConvUTF8).c_str()));
							wxLogVerbose(wxString::Format("	 Texture %d ", k));
							//			wxLogVerbose(wxString::Format("	 Name: \"%s\"", (char *) lTexture->GetName()));
							wxLogVerbose(wxString::Format("	 File Name: \"%s\"", (char *) fbxTextureFile->GetFileName()));
							wxLogVerbose(wxString::Format("	 Blend Mode: %s ", wxString(lBlendModes[lBlendMode], wxConvUTF8).c_str()));
							wxLogVerbose(wxString::Format("	 Alpha: %4.8f ", lTexture->GetDefaultAlpha()));
							wxLogVerbose(wxString::Format("	   UVSet: %s", lTexture->UVSet.Get().Buffer()));
							textureInfo.originalName  = wxFileName(wxString(fbxTextureFile->GetFileName(), wxConvUTF8));
							// TO DO :: message box
							if (!textureInfo.originalName.FileExists()) {
								const aqua::Locator::Service<SceneManager> &sceneManager(aqua::Locator::checkOut<SceneManager>());
								sceneManager->markAsUnexportable();
								aqua::Locator::checkIn(sceneManager);
								result = false;
								wxLogError("On Object %s ",	 wxString(pNode->GetName(), wxConvUTF8).c_str());
								wxLogError(wxString::Format("Texture File Name: \"%s\" Does not exist!", (char *) fbxTextureFile->GetFileName()));
								const aqua::Locator::Service<Config> &config(aqua::Locator::checkOut<Config>());
								textureInfo.originalName = config->projectMissingTexture();
								aqua::Locator::checkIn(config);
							}
							PBASSERT(textureInfo.originalName.FileExists(), "Missing texture");
							textureInfo.fileName	  =	stripTextureFileName(std::string(textureInfo.originalName.GetFullPath().c_str()));
							textureInfo.blendMode	 =	parseFbxBlendMode(lBlendMode);
							textureInfo.uvSetName	 =	std::string(lTexture->UVSet.Get().Buffer());
							textureInfo.alpha		 =	aqua::Float32(lTexture->GetDefaultAlpha());
						}
						materialTextureInfo.push_back(textureInfo);
					}
				}
			} else {
				//no layered texture simply get on the property
				int lNbTextures = lProperty.GetSrcObjectCount(FbxTexture::ClassId);
				PBASSERT(lNbTextures <= 1, "Untextured object");
				if (lNbTextures > 0) {
					//	wxLogVerbose(wxString::Format("Texture Channel	%s ", wxString(FbxLayerElement::TEXTURE_CHANNEL_NAMES[lTextureIndex], wxConvUTF8).c_str()));
					wxLogVerbose(wxString::Format("	 Textures for %s ",	 wxString(lProperty.GetName(), wxConvUTF8).c_str()));
				}
				for (int j =0; j<lNbTextures; ++j) {
					IMesh::TextureInfo textureInfo;
					FbxTexture* lTexture = FbxCast <FbxTexture> (lProperty.GetSrcObject(FbxTexture::ClassId,j));
					FbxTexture::EBlendMode lBlendMode = lTexture->GetBlendMode();
					wxLogVerbose(wxString::Format("	 Texture %d ", j));
					if (lTexture) {
						//		wxLogVerbose(wxString::Format("	 Name: \"%s\"", (char *) lTexture->GetName()));
						FbxFileTexture* fbxTextureFile(FbxCast<FbxFileTexture>(lTexture));
						wxLogVerbose(wxString::Format("	 File Name: \"%s\"", (char *) fbxTextureFile->GetFileName()));
						wxLogVerbose(wxString::Format("	 Blend Mode: %s ", wxString(lBlendModes[lBlendMode], wxConvUTF8).c_str()));
						wxLogVerbose(wxString::Format("	   UVSet: %s", lTexture->UVSet.Get().Buffer()));
						textureInfo.originalName  = wxFileName(wxString(fbxTextureFile->GetFileName(), wxConvUTF8));
						if (false == textureInfo.originalName.FileExists()) {
							textureInfo.originalName = wxFileName(wxString(fbxTextureFile->GetRelativeFileName(), wxConvUTF8)); // Try the Relative path as some exporters seem to use this instead ( Blender, for instance ) -- Steven Campbell
							if (false == textureInfo.originalName.FileExists()) {
								const aqua::Locator::Service<SceneManager> &sceneManager(aqua::Locator::checkOut<SceneManager>());
								sceneManager->markAsUnexportable();
								aqua::Locator::checkIn(sceneManager);
								result = false;
								wxLogError("On Object %s ", wxString(pNode->GetName(), wxConvUTF8).c_str());
								wxLogError(wxString::Format("Texure File Name: \"%s\" Does not exist!", (char *) fbxTextureFile->GetFileName()));
								const aqua::Locator::Service<Config> &config(aqua::Locator::checkOut<Config>());
								textureInfo.originalName = config->projectMissingTexture();
								aqua::Locator::checkIn(config);
							}
						}
						PBASSERT(textureInfo.originalName.FileExists(), "Missing texture: " << (char *) fbxTextureFile->GetFileName());
						textureInfo.fileName	  =	stripTextureFileName(std::string(textureInfo.originalName.GetFullPath().c_str()));
						textureInfo.blendMode	 = parseFbxBlendMode(FbxLayeredTexture::eOver);
						textureInfo.uvSetName	 = std::string(lTexture->UVSet.Get().Buffer());
						textureInfo.alpha		 = 1.0f;
					}
					materialTextureInfo.push_back(textureInfo);
				}
			}
		}
	}
	return result;
}

/**
 * Validate texture - ensure its something the game can render
 * @param pNode Node associated with this texture
 * @param textureImage The actual texture as an image
 * @param textureHasAlpha Flag to show if the texture should have an alpha channel
 * @param ti_it Iterator pointing to a texture info structure
 * @return true if ok
 */
bool validateTexture(FbxNode* pNode, wxImage& textureImage, bool& textureHasAlpha, std::vector<IMesh::TextureInfo>::iterator ti_it)
{
	bool buggeredTexture = false;
	// TO DO -- farm this out into checkTexture() function
	wxImage mirrorMe(ti_it->originalName.GetFullPath(), wxBITMAP_TYPE_TGA);
	if (false == mirrorMe.Ok()) // not a TGA file, try something else
		mirrorMe = wxImage(ti_it->originalName.GetFullPath(), wxBITMAP_TYPE_ANY);
	// do some validation of the texture
	PBASSERT(mirrorMe.Ok(), "Image is invalid!");
	if(!mirrorMe.Ok()) {
		wxLogError("On Object %s ", wxString(pNode->GetName(), wxConvUTF8).c_str());
		wxLogError(wxString::Format("%s is not a readable texture ", ti_it->originalName.GetFullPath()));

		buggeredTexture = true;
	}
	// check the dimensions
	PBASSERT((mirrorMe.GetHeight() >= 8) && (mirrorMe.GetWidth() >= 8), "Image is small");
	if((mirrorMe.GetHeight() < 8) && (mirrorMe.GetWidth() < 8)) {
		wxLogError("On Object %s ", wxString(pNode->GetName(), wxConvUTF8).c_str());
		wxLogError(wxString::Format("%s is too small ", ti_it->originalName.GetFullPath()));
		buggeredTexture = true;
	}
//	// check the height
//	PBASSERT((mirrorMe.GetHeight() & (mirrorMe.GetHeight() - 1)) == 0, "Height is not power of two");
//	if((mirrorMe.GetHeight() & (mirrorMe.GetHeight() - 1)) != 0) {
//		wxLogError("On Object %s ", wxString(pNode->GetName(), wxConvUTF8).c_str());
//		wxLogError(wxString::Format("%s height is not a power of two", ti_it->originalName.GetFullPath()));
//		buggeredTexture = true;
//	}
//	// check the width
//	PBASSERT((mirrorMe.GetWidth() & (mirrorMe.GetWidth() - 1)) == 0, "Image is not power of two");
//	if((mirrorMe.GetWidth() & (mirrorMe.GetWidth() - 1)) != 0) {
//		wxLogError("On Object %s ", wxString(pNode->GetName(), wxConvUTF8).c_str());
//		wxLogError(wxString::Format("%s width is not a power of two", ti_it->originalName.GetFullPath()));
//		buggeredTexture = true;
//	}
	// if it's borked, replace it with missing.tga
	if(buggeredTexture) {
		const aqua::Locator::Service<Config> &config(aqua::Locator::checkOut<Config>()); 
		ti_it->originalName = config->projectMissingTexture();
		aqua::Locator::checkIn(config);
		wxLogError("On Object %s ", wxString(pNode->GetName(), wxConvUTF8).c_str());
		wxLogError(wxString::Format("%s width is not a power of two", ti_it->originalName.GetFullPath()));
		// fall back on missing texture
		wxImage mirrorMe = wxImage(ti_it->originalName.GetFullPath(), wxBITMAP_TYPE_TGA);
		const aqua::Locator::Service<SceneManager> &sceneManager(aqua::Locator::checkOut<SceneManager>());
		sceneManager->markAsUnexportable();
		aqua::Locator::checkIn(sceneManager);							
	}
	// Does it has an alpha channel?
	textureHasAlpha = mirrorMe.HasAlpha();
	if(textureHasAlpha) {
		// if it has an alpha channel, check it's actually used
		bool hasRedundantAlpha = true;
		unsigned char* alphaData(mirrorMe.GetAlpha());
		const int dataSize(mirrorMe.GetHeight() * mirrorMe.GetWidth());
		for(int index(0U); index < dataSize; ++index) {
			unsigned char color(alphaData[index]);
			if(color != 0xFF) {
				hasRedundantAlpha = false;
				break;
			}				
		}
		if(hasRedundantAlpha) {
			textureHasAlpha = false;
			const aqua::Locator::Service<SceneManager> &sceneManager(aqua::Locator::checkOut<SceneManager>());
			sceneManager->markAsUnexportable();
			aqua::Locator::checkIn(sceneManager);				
			wxLogError(wxString::Format("%s has a redundant alpha channel.", ti_it->originalName.GetFullPath()));
			TextFile newText;
			const aqua::Locator::Service<Config> &config(aqua::Locator::checkOut<Config>()); 
			newText.openForAppending(config->projectTexturesDirectory() + wxString(wxT("TexturesToConvert.txt")));
			aqua::Locator::checkIn(config);
			newText.writeLine(ti_it->originalName.GetFullPath());
			newText.close();
		}
	} else {
		textureHasAlpha = mirrorMe.HasAlpha();
	}
	// flip the image - TGAS wrong way round
	textureImage = mirrorMe.Mirror(false);
	return buggeredTexture;
}	

Byte* textureData(const wxImage& image, Uint32& size) {
	Byte* result;

	size = image.GetWidth() * image.GetHeight() * 3;
	if (image.HasAlpha()) {
		size += image.GetWidth() * image.GetHeight();
	}
	result = new Byte[size];
	if (!image.HasAlpha()) {
		for(Uint32 i = 0; i < size; ++i) {
			result[i] = image.GetData()[i];
		}
	} else {
		int colData = 0;
		int alphaData = 0;
		for(Uint32 i = 0; i < size; ++i) {
			if ((i % 4) != 3) {
				result[i] = image.GetData()[colData];
				++colData;
			} else {
				result[i] = image.GetData()[alphaData];
				++alphaData;
			}
		}			
	}
	return result;
}
	
//! return the minimum extent of the bounding box
const PbVector3 MeshObject::getMinExtent() const {
	PbVector4 minExtent(minExtent_.x(), minExtent_.y(), minExtent_.z(), 1.0f);
	minExtent = *transformMatrix_ * minExtent;
	return PbVector3(minExtent.x(), minExtent.y(), minExtent.z());	
}

//! return the maximum extent of the bounding box
const PbVector3 MeshObject::getMaxExtent() const {
	PbVector4 maxExtent(maxExtent_.x(), maxExtent_.y(), maxExtent_.z(), 1.0f);
	maxExtent = *transformMatrix_ * maxExtent;
	return PbVector3(maxExtent.x(), maxExtent.y(), maxExtent.z());
}



