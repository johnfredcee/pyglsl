#include "StdAfx.h"

#include <Types.h>
#include <aqua/VertexBufferData.h>
#include <aqua/VertexIndexBufferData.h>
#include <aqua/SkeletonBufferData.h>
#include "IntermediateMesh.h"
#include "IMesh.h"
#include "MetaSerializer.h"
#include "SceneManager.h"
#include "MetaBufferManager.h"

using namespace aqua;



template<> void MetaSerialiser<VertexIndexBufferData>(const VertexIndexBufferData& data, DynamicMetaParser& parser)
{
	PBASSERT(parser.IsValid(), "Invalid index buffer parser");
	DynamicMetaParser indicesParser(parser.Find("indices_"));
	indicesParser.SetArrayCount(data.getNumIndices());
	for(Uint32 index = 0; index < data.getNumIndices(); ++index) {
		indicesParser.Find(index, MetaParser::FIND_BY_INDEX).SetValue(data.getStaticIndexData()[index]);
	}
	return;	
}

template<> void MetaSerialiser<VertexBufferData>(const VertexBufferData& data, DynamicMetaParser& parser) {

	PBASSERT(parser.IsValid(), "Invalid vertex buffer parser");

	Uint32 format = data.getFormat();
	
	parser.Find("format_").SetValue(format);
	parser.Find("numVertices_").SetValue(data.getNumVertices());
	parser.Find("containsVertexAlphas_").SetValue(data.isAlphaColorEnabled());; 
	parser.Find("strideSize_").SetValue(VertexBufferData::calculateStrideSize(format));
	parser.Find("data_").CopyBinBlock(data.getData(), data.getDataSize());
}

template<> void MetaSerialiser<SkeletonBufferData>(const SkeletonBufferData& data, DynamicMetaParser& parser) {
	PBASSERT(parser.IsValid(), "Invalid vertex buffer parser");
	parser.Find("vBufferId_").SetValue(data.vBufferId_);
	parser.Find("verts_").SetArrayCount(data.verts_.size());
	for(aqua::Uint32 i = 0; i < data.verts_.size(); ++i) {
		DynamicMetaParser vertParser = parser.Find("verts_").Find(i, MetaParser::FIND_BY_INDEX);
		BoneInfluence influence = data.verts_[i];
		vertParser.Find("originalX").SetValue(influence.originalX);
		vertParser.Find("originalY").SetValue(influence.originalY);
		vertParser.Find("originalZ").SetValue(influence.originalZ);

		vertParser.Find("weight0").SetValue(influence.weight0);
		vertParser.Find("weight1").SetValue(influence.weight1);
		vertParser.Find("weight2").SetValue(influence.weight2);

		vertParser.Find("boneIndex1").SetValue(influence.boneIndex1);
		vertParser.Find("boneIndex2").SetValue(influence.boneIndex2);
		vertParser.Find("boneIndex3").SetValue(influence.boneIndex3);
		vertParser.Find("boneIndex4").SetValue(influence.boneIndex4);
	}
}

MetaBufferManager::MetaBufferManager() {
}

MetaBufferManager::~MetaBufferManager() {
}

bool MetaBufferManager::checkBuffers(const std::string&name,
									 bool& instancedVertices,
									 bool& instancedIndices) {
	const Locator::Service<SceneManager>& scene(Locator::checkOut<SceneManager>());
	std::string vbufferName(wxString::Format(wxT("%s_vertices"), name.c_str()).utf8_str());
	instancedVertices = scene->existsInScene(vbufferName);	   
	std::string ibufferName(wxString::Format(wxT("%s_indices"), name.c_str()).utf8_str());	  
	instancedIndices = scene->existsInScene(ibufferName);
	Locator::checkIn(scene);
	return true;				
}

void MetaBufferManager::computeMaterialStats(shared_ptr<OpenMesh::IntermediateMesh> iMesh,
											 int  materialIndex,
											 int& materialPolygonCount,
											 int& materialVertexCount,
											 bool& usesUv1,
											 bool& usesUv2) {

	OpenMesh::HPropHandleT<OpenMesh::IntermediateMesh::TexCoord2D> uv2s;
	if (!iMesh->get_property_handle(uv2s, "uv2")) {
		PBBREAK("Something wrong");
	}	
	// first, work out number of faces affected by material
	materialPolygonCount = 0;
	materialVertexCount = 0;
	OpenMesh::IntermediateMesh::FaceIter f_it;
	OpenMesh::IntermediateMesh::VertexHandle lastVHandle;
	for (f_it = iMesh->faces_begin(); f_it != iMesh->faces_end(); ++f_it) {
		if ((materialIndex == -1) || (iMesh->texture_index(f_it) == materialIndex)) {
			OpenMesh::IntermediateMesh::FaceHalfedgeIter fh_it;
			// check the face to see which uv channels it uses
			for (fh_it = iMesh->fh_iter(f_it.handle()); fh_it; ++fh_it) {
				OpenMesh::IntermediateMesh::VertexHandle vHandle(iMesh->to_vertex_handle(fh_it.handle()));
				OpenMesh::IntermediateMesh::TexCoord2D oldUv = iMesh->texcoord2D(fh_it.handle());
				usesUv1 = (!isinf(oldUv[0]));
				oldUv = iMesh->property(uv2s, fh_it.handle());
				usesUv2 = (!isinf(oldUv[0]));
				if (vHandle != lastVHandle) {
					++materialVertexCount;
					lastVHandle = vHandle;
				}
			}
			++materialPolygonCount;
		}
	}
	wxLogDebug("Mesh has %d polygons, and %d vertices.", materialPolygonCount, materialVertexCount);
}

/**
 * Compute a new remapping of vertex indices for a given material
 * @param iMesh Mesh 
 * @param oldIndices Map from new indices to old
 * @param indexData New indices (suitable for vertex index buffers)
 * @param materialIndex Material to remap for
 */
void MetaBufferManager::computeIndexRemapping(shared_ptr<OpenMesh::IntermediateMesh> iMesh,
											  std::vector<int>& oldIndices,
											  std::vector<IndexData>&  indexData,
											  int materialIndex,
											  int materialVertexCount) 											  
{
	const unsigned int halfEdgeCount(materialVertexCount * 3);
	// indices as raw uint32 array one entry for each material
	indexData.clear();
	indexData.reserve(halfEdgeCount);
	// index of newly created vertex 
	unsigned int newVertexIndex(0U);
	// use this to map old indices to new indices
	std::vector<int> newIndices(halfEdgeCount, -1);
	// and vice - versa
	oldIndices.reserve(halfEdgeCount);
	// begin face walk to recompute vertex indices
	OpenMesh::IntermediateMesh::FaceIter f_it;	
	for (f_it = iMesh->faces_begin(); f_it != iMesh->faces_end(); ++f_it) {
		
		OpenMesh::IntermediateMesh::FaceHandle fHandle(f_it.handle());
		OpenMesh::IntermediateMesh::FaceHalfedgeIter fv_it;
		// does this face use the right material?
		if ((materialIndex != -1) && (iMesh->texture_index(f_it) != materialIndex))
			continue;
		// for each vetex in the face
		for (fv_it = iMesh->fh_iter(f_it.handle()); fv_it; ++fv_it) {
			// what was the primitive index of the vertex in the mesh?
			int primitiveIndex(fv_it.handle().idx());
			PBASSERT(((primitiveIndex >= 0) &&
					  (primitiveIndex < halfEdgeCount)), "Index '" << primitiveIndex << "' out of range of edge count " << halfEdgeCount);
			// have we seen this index before?
			if (newIndices[primitiveIndex] == -1) {
				// no, create a new mapping : the primitive vertex index to the newly created vertex index
				newIndices[primitiveIndex] = newVertexIndex;
				// and vice versa
				oldIndices.push_back(primitiveIndex);
				newVertexIndex++;
			}
			// add an index to the list at the indexed vertex (whether seen before or newly allocated)
			indexData.push_back(aqua::IndexData(newIndices[primitiveIndex]));
		}
	}	
}

aqua::shared_ptr<VertexBufferData> MetaBufferManager::fillVertexBuffer(const std::string& name,
																   bool instancedVertices,
																   Array<IndexData>& indices,
																   Array<BoneInfluence>& influences,
																   bool instancedIndices,
																   shared_ptr<OpenMesh::IntermediateMesh> iMesh,
																   int materialIndex) {
	PBUNREFERENCED_PARAMETER(name);
	// TODO -- if either of these are true, we should probably
	// fish out the existing buffers and return a pointer to them
	PBUNREFERENCED_PARAMETER(instancedVertices);
	PBUNREFERENCED_PARAMETER(instancedIndices);
	
	// check the buffer and iMesh attributes match up
	Uint32 format = VertexBufferData::PRIMITIVE;
	if (iMesh->has_vertex_normals()) {
		format = format | (int) VertexBufferData::NORMAL;
	}
	if (iMesh->has_vertex_colors()) {
		format =  format | (int) VertexBufferData::COLOR;
	}
	bool usesUv1 = false;
	bool usesUv2 = false;
	// first, work out number of faces affected by material
	int materialPolygonCount = 0;
	int materialVertexCount = 0;	
	computeMaterialStats(iMesh, materialIndex, materialPolygonCount, materialVertexCount, usesUv1, usesUv2);
	if (usesUv1) {
		format = format | (int) VertexBufferData::TEXTURE_COORDINATES;
	}
	if (usesUv2) {
		format = format | (int) VertexBufferData::TEXTURE_COORDINATES2;
	}
	wxLogVerbose("Making buffers for material %d ", materialIndex);
	wxLogVerbose("Material is on %d vertices and %d polygons", materialVertexCount, materialPolygonCount);
	// now fill in data, remapping indices as we go
	//wxLogVerbose("Fill buffers for material %d ", materialIndex);
	
	// indices as raw uint32 array one entry for each material
	std::vector<IndexData>  indexData;
	indexData.reserve(materialVertexCount);
	// and vice - versa
	std::vector<int> oldIndices;
	oldIndices.reserve(materialVertexCount);
	// begin face walk to recompute vertex indices
	computeIndexRemapping(iMesh, oldIndices, indexData, materialIndex, materialVertexCount);
	// create iterators for start of walk
	// vertices as raw float array for optimisation
	aqua::Array<PrimitiveData>			primitives(oldIndices.size());
	primitives.resize(oldIndices.size());
	aqua::Array<NormalData>				normals(oldIndices.size());
	if (format & VertexBufferData::NORMAL) 
		normals.resize(oldIndices.size());
	aqua::Array<TextureCoordinateData>	textureCoordinate1(oldIndices.size());
	if (format & VertexBufferData::TEXTURE_COORDINATES) 
		textureCoordinate1.resize(oldIndices.size());
	aqua::Array<TextureCoordinateData>	textureCoordinate2(oldIndices.size());
	if (format & VertexBufferData::TEXTURE_COORDINATES2) 
		textureCoordinate2.resize(oldIndices.size());
	aqua::Array<VertexColorData>		colors(oldIndices.size());
	if (format & VertexBufferData::COLOR) 
		colors.resize(oldIndices.size());

	PrimitiveData*				primItr(primitives.begin());
	NormalData*					normalItr(normals.begin());
	TextureCoordinateData*		uv1Itr(textureCoordinate1.begin());
	TextureCoordinateData*		uv2Itr(textureCoordinate2.begin());
	VertexColorData*			colorItr(colors.begin());

	OpenMesh::HPropHandleT<OpenMesh::IntermediateMesh::TexCoord2D> uv2s;
	if (!iMesh->get_property_handle(uv2s, "uv2")) {
		PBBREAK("Something wrong");
	}	
	
	// vertex walk to fill in vertex buffer
	for (std::vector<int>::iterator vertexIndexIterator = oldIndices.begin();
		 vertexIndexIterator != oldIndices.end();
		 ++vertexIndexIterator) {
		OpenMesh::IntermediateMesh::HalfedgeHandle hfHandle(iMesh->halfedge_handle(*vertexIndexIterator));
		OpenMesh::IntermediateMesh::VertexHandle vHandle(iMesh->to_vertex_handle(hfHandle));
		OpenMesh::IntermediateMesh::Point pt = iMesh->point(vHandle);
		PBASSERT(primItr != primitives.end(), "Iterated off end of buffer");
		primItr->x = pt[0];
		primItr->y = pt[1];
		primItr->z = pt[2];
		primItr++;
		if (format & VertexBufferData::NORMAL) {
			OpenMesh::IntermediateMesh::Normal norm = iMesh->normal(vHandle);
			normalItr->x = norm[0];
			normalItr->y = norm[1];
			normalItr->z = norm[2];
			normalItr++;
		}
		if (format & VertexBufferData::COLOR) {
			OpenMesh::IntermediateMesh::Color color = iMesh->color(vHandle);
			colorItr->r = aqua::Uint8((color[0] * (1.0f / 255.0f)));
			colorItr->g = aqua::Uint8((color[1] * (1.0f / 255.0f)));
			colorItr->b = aqua::Uint8((color[2] * (1.0f / 255.0f)));
			colorItr->a = aqua::Uint8((color[3] * (1.0f / 255.0f)));
			colorItr++;
		}
		if (format & VertexBufferData::TEXTURE_COORDINATES) {
			OpenMesh::IntermediateMesh::TexCoord2D uv = iMesh->texcoord2D(hfHandle);
			uv1Itr->u = uv[0];
			uv1Itr->v = uv[1];
			uv1Itr++;
		}
		if (format & VertexBufferData::TEXTURE_COORDINATES2) {
			OpenMesh::IntermediateMesh::TexCoord2D uv = iMesh->property(uv2s, hfHandle);
			uv2Itr->u = uv[0];
			uv2Itr->v = uv[1];
			uv2Itr++;
		}
	} // for each face
	indices.resize(indexData.size());
	int i = 0;
	for (std::vector<IndexData>::iterator vertexIndexIterator = indexData.begin();
		 vertexIndexIterator != indexData.end();
		 ++vertexIndexIterator, ++i) {
		indices[i] =  *vertexIndexIterator;
	}
	// fill in skeleton info, if any
	influences.clear();
	OpenMesh::VPropHandleT<OpenMesh::IntermediateMesh::Point> ovs;
	bool hasSkeletonData = iMesh->get_property_handle(ovs,"originalVerts");
	// does this mesh have skeleton data
	if (hasSkeletonData) {
		OpenMesh::VPropHandleT<OpenMesh::IntermediateMeshTraits::BoneIndex> iBones;
		iMesh->get_property_handle(iBones, "boneIndexes");
		OpenMesh::VPropHandleT<OpenMesh::IntermediateMeshTraits::Weight> weights;
		iMesh->get_property_handle(weights, "boneWeights");
		for (std::vector<int>::iterator vertexIndexIterator = oldIndices.begin();
			 vertexIndexIterator != oldIndices.end();
			 ++vertexIndexIterator) {
			OpenMesh::IntermediateMesh::VertexHandle vHandle(iMesh->vertex_handle(*vertexIndexIterator));
			OpenMesh::IntermediateMeshTraits::BoneIndex boneIndices = iMesh->property(iBones, vHandle);
			OpenMesh::IntermediateMeshTraits::Weight boneWeights = iMesh->property(weights, vHandle);
			OpenMesh::IntermediateMeshTraits::Point originalVert = iMesh->property(ovs, vHandle);
			BoneInfluence influence(originalVert[0], originalVert[1], originalVert[2],
									boneWeights[0], boneWeights[1], boneWeights[2],
									boneIndices[0], boneIndices[1], boneIndices[2], boneIndices[3]);
			influences.pushBack(influence);	
		}
	}
	return shared_ptr<VertexBufferData>(new VertexBufferData(primitives, normals, textureCoordinate1, textureCoordinate2, colors));									
}
	
bool MetaBufferManager::addBuffers(const std::string& name,
								   bool instancedVertices,
								   aqua::shared_ptr<VertexBufferData> vertices,
								   bool instancedIndices,
								   Array<IndexData> indices,
								   Array<BoneInfluence> influences) {

	PBUNREFERENCED_PARAMETER(instancedIndices);
	PBUNREFERENCED_PARAMETER(instancedVertices);
	
	const Locator::Service<SceneManager>& scene(Locator::checkOut<SceneManager>());
	std::string vbufferName(wxString::Format(wxT("%s_vertices"), name.c_str()).utf8_str());
	std::string vertexBufferType("VertexBufferData");
	DynamicMetaParser vbufferParser(scene->allocateData(vbufferName, vertexBufferType));
	MetaSerialiser<VertexBufferData>(*vertices, vbufferParser);
	scoped_ptr<VertexIndexBufferData> ibuffer(new VertexIndexBufferData(indices));
	std::string ibufferName(wxString::Format(wxT("%s_indices"), name.c_str()).utf8_str());
	std::string vertexIndexBufferType("VertexIndexBufferData");	
	DynamicMetaParser ibufferParser(scene->allocateData(ibufferName, vertexIndexBufferType));	
	MetaSerialiser<VertexIndexBufferData>(*ibuffer, ibufferParser);
	if (influences.size() != 0) {	
		std::string skeletonBufferType("SkeletonBufferData");	
		std::string sbufferName(wxString::Format(wxT("%s_skeleton"), name.c_str()).utf8_str());
		scoped_ptr<SkeletonBufferData> sBuffer(new SkeletonBufferData(aqua::HashString(vbufferName.c_str()), influences));
		DynamicMetaParser sBufferParser(scene->allocateData(sbufferName, skeletonBufferType));
		MetaSerialiser<SkeletonBufferData>(*sBuffer, sBufferParser);		
	}
	Locator::checkIn(scene);
	return true;		
}


crc32 vbufferId(const std::string& name) {
	std::string vbufferName(wxString::Format(wxT("%s_vertices"), name.c_str()).utf8_str());		
	return CalcCRC(vbufferName.c_str());	
}

crc32 ibufferId(const std::string& name) {
	std::string ibufferName(wxString::Format(wxT("%s_indices"), name.c_str()).utf8_str());	  
	return CalcCRC(ibufferName.c_str());
}
