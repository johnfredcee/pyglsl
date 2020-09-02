#include "StdAfx.h"


#include <PbSystemTypes.h>
#include <aqua/Material.h>
#include "Layers.h"
#include "ViewerFrame.h"
#include "FbxParse.h"

using namespace aqua;


/**
 * Constructor
 * @param scene FbxScene we wish to parse
 */
FbxParse::FbxParse(FbxScene* scene) : scene_(scene)
{
};


FbxParse::~FbxParse()
{
}

/**
 * Fill a vector with pointers to the fbx display layers in the scene
 * @param layers vector of FbxDisplayLayer*'s (might be empty if none)
 */
void FbxParse::layers(std::vector<FbxDisplayLayer*>& layers)
{
	layers.clear();
	int size = scene_->GetMemberCount(FbxCriteria::ObjectIsA(FbxDisplayLayer::ClassId));
	layers.reserve(size);
	for(int i = 0; i < size; i++) {
		layers.push_back(FbxCast<FbxDisplayLayer>(scene_->GetMember(FbxCriteria::ObjectIsA(FbxDisplayLayer::ClassId), i)));
	}
}

/**
 * Return a list of layer names in the scene
 * @param layerNames vector to store layer names in
 */
void FbxParse::layerNames(std::vector<std::string>& layerNames)
{
	wxASSERT(scene_ != NULL);
	layerNames.clear();
	int size = scene_->GetMemberCount(FbxCriteria::ObjectIsA(FbxDisplayLayer::ClassId));
	layerNames.reserve(size);
	for(int i = 0; i < size; ++i) {
		FbxObject* object = scene_->GetMember(FbxCriteria::ObjectIsA(FbxDisplayLayer::ClassId), i);
		if(object) {
			FbxDisplayLayer *layer	 = FbxCast<FbxDisplayLayer>(object);
			wxLogDebug("Found layer %s", layer->GetName());
			layerNames.push_back(std::string(layer->GetName()));;
		}
	}
	return;
}

/**
 * Fill a vector of node pointers with the nodes of the objects inside the display layers
 * @param layer Layer to
 * @param nodes Vector of nodes to fill
 */
void FbxParse::getLayerNodes(FbxDisplayLayer* layer, std::vector<FbxNode*>& nodes)
{
	wxASSERT(layer != NULL);
	nodes.clear();
	int size = layer->GetMemberCount(FbxCriteria::ObjectType(FbxNode::ClassId));
	nodes.reserve(size);
	for(int i = 0; i < size; ++i) {
		FbxObject* object = layer->GetMember(FbxCriteria::ObjectType(FbxNode::ClassId), i);
		if(object) {
			FbxNode* node = FbxCast<FbxNode>(object);
			nodes.push_back(node);
		}
	}
}

/**
 * Fill a vector of node pointers with the nodes of the objects inside the display layers
 * @param layer Layer to
 * @param nodes Vector of nodes to fill
 */
void FbxParse::getNodes(FbxClassId& type, std::vector<FbxNode*>& nodes)
{
	wxASSERT(scene_ != NULL);
	nodes.clear();
	int size = scene_->GetMemberCount(FbxCriteria::ObjectType(FbxNode::ClassId));
	nodes.reserve(size);
	for(int i = 0; i < size; ++i) {
		FbxObject* object = scene_->GetMember(FbxCriteria::ObjectType(type), i);
		if(object) {
			FbxNode* node = FbxCast<FbxNode>(object);
			nodes.push_back(node);
		}
	}
}

/**
 * GetDisplayLayer
 * Returns the display layer ( == Max layer) a given node is in
 * @param pNode Node to return layer of
 * @return FbxDisplayLayer* Pointer to display layer of node
 */
FbxDisplayLayer* FbxParse::getDisplayLayer(FbxNode* pNode)
{
	int layers = pNode->GetDstObjectCount(FbxDisplayLayer::ClassId);
	if(layers == 0)
		return NULL;
	FbxObject *layer = pNode->GetDstObject(FbxDisplayLayer::ClassId, 0);
	wxASSERT(layer != NULL);
	return FbxCast< FbxDisplayLayer >(layer);
}

/**
 * getShaderName
 * return the shader portion of a material name
 * @param name material name
 * @return shader name portion of name
 */
std::string FbxParse::getShaderName(const std::string& name)
{
	size_t pos = name.find_first_of('#');
	return (pos == std::string::npos) ? std::string("simple_shader") : name.substr(0, pos);
}


/**
 * Produce a simple string describing the nature of the node
 * @param pNode node to describe
 * @return description string
 */
wxString getNodeDescription(FbxNode* pNode)
{
	wxString result;
	FbxNodeAttribute::EType lAttributeType;
	if(pNode->GetNodeAttribute() == NULL) {
		FbxProperty isSplineProperty(pNode->FindProperty("IsSpline"));
		result = isSplineProperty.IsValid() ? wxT(" (Spline)") : wxT(" (No node attribute type)");
	} else {
		lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());
		switch(lAttributeType) {
			case FbxNodeAttribute::eMarker:
				result = wxT(" (Marker)");
				break;
			case FbxNodeAttribute::eSkeleton:
				result = wxT(" (Bone)");
				break;
			case FbxNodeAttribute::eMesh:
				result = wxT(" (Mesh)");
				break;
			case FbxNodeAttribute::eCamera:
				result = wxT(" (Camera)");
				break;
			case FbxNodeAttribute::eCameraStereo:
				result = wxT(" (Camera Stereo)");
				break;
			case FbxNodeAttribute::eLight:
				result = wxT(" (Light)");
				break;
			case FbxNodeAttribute::eBoundary:
				result = wxT(" (Boundary)");
				break;
			case FbxNodeAttribute::eOpticalMarker:
				result = wxT(" (Optical Marker)");
				break;
			case FbxNodeAttribute::eOpticalReference:
				result = wxT(" (Optical Reference)");
				break;
			case FbxNodeAttribute::eCameraSwitcher:
				result = wxT(" (Camera Switcher)");
				break;
			case FbxNodeAttribute::eNull: {
				FbxProperty isSplineProperty(pNode->FindProperty("IsSpline"));
				result = (isSplineProperty.IsValid()) ?	 wxT(" (Spline)") : wxT(" (Null)");
			}
				break;
			case FbxNodeAttribute::ePatch:
				result = wxT(" (Patch)");
				break;
			case FbxNodeAttribute::eNurbs:
				result = wxT(" (Nurbs)");
				break;
			case FbxNodeAttribute::eNurbsSurface:
				result = wxT(" (Nurbs Surface)");
				break;
			case FbxNodeAttribute::eNurbsCurve:
				result = wxT(" (Nurbs Curve)");
				break;
			case FbxNodeAttribute::eTrimNurbsSurface:
				result = wxT(" (Trim Nurbs Surface)");
				break;
			case FbxNodeAttribute::eShape:
				result = wxT(" (Shape)");
				break;
			case FbxNodeAttribute::eLODGroup:
				result = wxT(" (LOD Group)");
				break;
			case FbxNodeAttribute::eSubDiv:
				result = wxT(" (Subdivision Surface)");
				break;
			case FbxNodeAttribute::eUnknown:
				result = wxT(" (Unknown)");
				break;
		}
	}
	return result;
}

/**
 * FbxToPlayboxTransform
 * Convert between a FBX matrix to a Playbox matrix taking into
 * account different conventions
 * @param transform transform to convert
 * @return shared_ptr<PbMatrix4> converted transform
 */
shared_ptr<PbMatrix4> fbxToPlayboxTransform(const FbxAMatrix& transform)
{
	aqua::Float32 rawValues[16];
	// I probably deserve to be shot for writing this, but it's the quickest way to deal with the mess
	for(const double *xformvalue = (const double*) transform; xformvalue < ((const double*) transform) + 16; ++xformvalue) {
		int index = xformvalue - (const double*) transform;
		rawValues[index] = aqua::Float32(*xformvalue);
	}
	shared_ptr<PbMatrix4> sceneTransform(PBNEW(MEM_DEFAULT) PbMatrix4((PbReal*) rawValues));
	sceneTransform->transpose();
	return sceneTransform;
}


// Get the global position.
// Do not take in account the geometric transform.
FbxAMatrix FbxParse::getGlobalDefaultTransform(FbxNode* pNode, KTime& pTime, FbxAMatrix* pParentGlobalTransform)
{
	// Ideally this function would use parent global position and local position to
	// compute the global position.
	// Unfortunately the equation
	//	  lGlobalPosition = pParentGlobalPosition * lLocalPosition
	// does not hold when inheritance type is other than "Parent" (RSrs). To compute
	// the parent rotation and scaling is tricky in the RrSs and Rrs cases.
	// This is why GetNodeGlobalTransform() is used: it always computes the right
	// global position.
	(void) pParentGlobalTransform;
	return pNode->GetScene()->GetEvaluator()->GetNodeGlobalTransform(pNode, pTime);
}


FbxAnimStack* FbxParse::setAnimationContext(KString* animStackName, KTime& start, KTime& stop)
{
	FbxAnimStack *result  = scene_->FindMember(FBX_TYPE(FbxAnimStack), animStackName->Buffer());
	if(result != NULL) {
		scene_->GetEvaluator()->SetContext(result);
		FbxTakeInfo* lCurrentTakeInfo = scene_->GetTakeInfo(*animStackName);
		if(lCurrentTakeInfo) {
			start = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
			stop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
		} else {
			// Take the time line value
			KTimeSpan lTimeLineTimeSpan;
			scene_->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);
			start = lTimeLineTimeSpan.GetStart();
			stop  = lTimeLineTimeSpan.GetStop();
		}
	}
	return result;
}

//!
//!	 Get the geometry deformation local to a node. It is never inherited by the children.
//! @param pNode node to get geometry
//! @return
//!
FbxAMatrix getGeometryTransform(FbxNode* pNode)
{
	FbxVector4 lT, lR, lS;
	FbxAMatrix lGeometry;
	lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
	lGeometry.SetT(lT);
	lGeometry.SetR(lR);
	lGeometry.SetS(lS);
	return lGeometry;
}

/**
 * Return the transform associated with a node (localTransform * geometryTransform)
 * @param pNode
 * @return FbxAMatrix
 */
FbxAMatrix getNodeTransform(FbxNode* node)
{
	FbxAMatrix transform;
	transform.SetIdentity();
	if (node != NULL) {
		FbxAMatrix lGlobalTransform;
		lGlobalTransform.SetIdentity();
		FbxAMatrix lGeometryOffset;
		lGeometryOffset.SetIdentity();
		lGlobalTransform = node->GetScene()->GetEvaluator()->GetNodeGlobalTransform(node);
		lGeometryOffset = getGeometryTransform(node);
		transform = lGlobalTransform * lGeometryOffset;
	}
	return transform;
}

/**
 * Return the transform associated with a node.
 * @param pNode
 * @return FbxAMatrix
 */
FbxAMatrix getNodeLocalTransform(FbxNode* node)
{
	FbxAMatrix transform = getNodeTransform(node);
	if (node != NULL) {
		FbxNode*	parent(node->GetParent());
		FbxAMatrix parentTransform;
		parentTransform.SetIdentity();
		if (parent != NULL)
			getNodeTransform(parent);
		FbxAMatrix inverseParentTransform(parentTransform.Inverse());
		transform *= inverseParentTransform;
	}
	return transform;
}

/**
 * Description for getAnimatedNodeTransform.
 * @param node Node to sample trasnform of
 * @param time time to sample node transform at
 * @return sampled FbxAMatrix
 */
FbxAMatrix FbxParse::getAnimatedNodeTransform(FbxNode* node, KTime time)
{
	//FbxAnimEvaluator* evaluator = node->GetScene()->GetEvaluator();
	FbxAMatrix lGlobalTransform;
	lGlobalTransform.SetIdentity();
	FbxAMatrix lGeometryOffset;
	lGeometryOffset.SetIdentity();
	lGlobalTransform = node->GetScene()->GetEvaluator()->GetNodeGlobalTransform(node, time);
	lGeometryOffset = getGeometryTransform(node);
	FbxAMatrix transform = lGlobalTransform * lGeometryOffset;
	return transform;
}


/**
 * Parse the layers associated with the mesh and build up an index of materials and uvSets which are actually used
 * @param pNode Node to parse
 * @param pMesh Pointer to mesh associated with node
 * @param usedMateraials <doc>
 * @param usedMaterialsIndex <doc>
 * @param uvSetNames <doc>
 */
void parseLayers(FbxNode* pNode, FbxMesh* pMesh, std::vector<int>& usedMaterials, std::vector<int>& usedMaterialsIndex, std::vector<std::string>& uvSetNames)
{
	PBUNREFERENCED_PARAMETER(pNode);
	// scan to find what materials are actually used
	int polygonCount = pMesh->GetPolygonCount();
	// check that the layer actually has materials (some are uvset only for multi-texture)
	LayerElementAccessor<FbxLayerElementMaterial, FbxSurfaceMaterial*> materialLayer(pMesh, 0, FbxLayerElement::eMaterial, false);
	if((!materialLayer.isValid()) || (materialLayer.reference() == FbxLayerElement::eIndex)) {
		wxLogVerbose("No Materials");
		usedMaterialsIndex.clear();
	} else {
		// build a "used materials index array" which contains the indices of materials actually *used* by the mesh per-polygon
		usedMaterialsIndex.resize(polygonCount);
		for(int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex) {
			int materialConnectionIndex = materialLayer.index(polygonIndex);
			if(std::find(usedMaterials.begin(), usedMaterials.end(), materialConnectionIndex) == usedMaterials.end()) {
				usedMaterials.push_back(materialConnectionIndex);
			}
			usedMaterialsIndex[polygonIndex] = std::find(usedMaterials.begin(), usedMaterials.end(), materialConnectionIndex) - usedMaterials.begin();
		}
	}
	wxLogVerbose("Mesh has %d Materials", usedMaterials.size());
	for(int iLayer = 0; iLayer < pMesh->GetLayerCount(); iLayer++) {
		wxLogVerbose("Layer #%d", iLayer);
		// find uv sets used by this material
		int uvSetCount = pMesh->GetLayer(iLayer)->GetUVSetCount();
		wxLogVerbose("Layer has %d UVSets ", uvSetCount);
		if(uvSetCount != 0) {
			FbxArray<FbxLayerElementUV const*> uvSets = pMesh->GetLayer(iLayer)->GetUVSets();
			for(int uvSetIndex = 0; uvSetIndex < pMesh->GetLayer(iLayer)->GetUVSetCount(); ++uvSetIndex) {
				wxLogVerbose(wxString::Format("Layer %d Set %d Name %s", iLayer, uvSetIndex, uvSets[uvSetIndex]->GetName()));
				std::string uvSetName(uvSets[uvSetIndex]->GetName());
				// add it only if it does not already exist to weed out duplicates
				if(std::find(uvSetNames.begin(), uvSetNames.end(), uvSetName) == uvSetNames.end())
					uvSetNames.push_back(uvSetName);
			}
		}
	}
}

/**
 * getShaderNamePart
 * return the shader portion of a material name
 * @param name material name
 * @return shader name portion of name
 */
std::string getShaderNamePart(const std::string& name)
{
	size_t pos = name.find_first_of('#');
	return (pos == std::string::npos) ? std::string("flat_shader") : name.substr(pos+1);
}

/**
 * Get the material name part of a material name
 * @param name Full name of material
 * @return material portion of name
 */
std::string getMaterialNamePart(const std::string& name)
{
	size_t pos = name.find_first_of('#');
	return (pos == std::string::npos) ? name : name.substr(0, pos);
}

/**
 * Parse an individial material associated with a node and some
 * geometry and fill in	 a blue::Material
 * @param material resulting material
 * @param pNode scene node mesh is attached to
 * @param mesh fbx mesh to read geometry from
 * @param materialIndex index of material to parse
 * @param name name of material
 * @param shaderName - name of shader embedded in material name
 */
void parseFbxNodeMaterial(aqua::shared_ptr<Material> material,
						  FbxNode* pNode,
						  FbxMesh* mesh,
						  int materialIndex,
						  std::string& name,
						  std::string& shaderName)
{
	PBUNREFERENCED_PARAMETER(mesh);
	FbxDouble3 lFbxDouble3;
	FbxDouble lFbxDouble1;
	FbxSurfaceMaterial *lMaterial	= pNode->GetMaterial(materialIndex);
	name = std::string(lMaterial->GetName());
	shaderName = getShaderNamePart(name);
	std::string materialName = getMaterialNamePart(name);
	name = materialName;
	material->shader = HashString(aqua::PbString(shaderName.c_str()));
	wxLogVerbose("Material	named %s  ", materialName.c_str());
	wxLogVerbose("Shader	named %s ",	 shaderName.c_str());
	if(lMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId)) {
		wxLogVerbose("Lambert Material %d ", materialIndex);
		lFbxDouble3 =((FbxSurfaceLambert *)lMaterial)->Diffuse;
		material->diffuse.setR(aqua::Float32(lFbxDouble3[0]));
		material->diffuse.setG(aqua::Float32(lFbxDouble3[1]));
		material->diffuse.setB(aqua::Float32(lFbxDouble3[2]));
		lFbxDouble1=((FbxSurfaceLambert *)lMaterial)->DiffuseFactor;
		material->diffuse.setA(aqua::Float32(lFbxDouble1));
		// lambert doesn't do specular (yes, this is horrible hack -- will go into different fields in the meta-Material)
		material->specular.factor = 0.1f;
		material->specular.power  = 0.01f;
		// compute emissive value from color intensity -- cannibalize ambient red channel
		lFbxDouble3=((FbxSurfaceLambert *)lMaterial)->Emissive;
		Float32 intensity = aqua::Float32(sqrt((lFbxDouble3[0] * lFbxDouble3[0]) +
											   (lFbxDouble3[1] * lFbxDouble3[1]) +
											   (lFbxDouble3[2] * lFbxDouble3[2])) / sqrt(3.0));
		material->emissiveLevel = intensity;
		wxLogError("Material %s	 ", name.c_str());
		wxLogError("Lambert: No specular component (should be Blinn or Phong Material).");
	} else if(lMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))	{
		wxLogVerbose("Phong Material %d ", materialIndex);
		lFbxDouble3 =((FbxSurfacePhong *)lMaterial)->Diffuse;
		material->diffuse.setR(aqua::Float32(lFbxDouble3[0]));
		material->diffuse.setG(aqua::Float32(lFbxDouble3[1]));
		material->diffuse.setB(aqua::Float32(lFbxDouble3[2]));
		lFbxDouble1=((FbxSurfacePhong *)lMaterial)->DiffuseFactor;
		material->diffuse.setA(aqua::Float32(lFbxDouble1));
		lFbxDouble1=((FbxSurfacePhong *)lMaterial)->SpecularFactor;
		Float32 factor = aqua::Float32(lFbxDouble1);
		if(!((factor <= 1.0f) && (factor >= 0.0f))) {
			wxLogError("Material %s	 ", name.c_str());
			wxLogError("Specular Factor %8.4f ", lFbxDouble1);
			wxLogError("Specular Factor out of range");
			factor = 0.98f;
		}
		material->specular.factor = factor;
		// compute emissive value from color intensity
		lFbxDouble3=((FbxSurfacePhong *)lMaterial)->Emissive;
		Float32 intensity = aqua::Float32(sqrt((lFbxDouble3[0] * lFbxDouble3[0]) +
											   (lFbxDouble3[1] * lFbxDouble3[1]) +
											   (lFbxDouble3[2] * lFbxDouble3[2])));
		material->emissiveLevel = intensity;
		lFbxDouble1 =((FbxSurfacePhong *) lMaterial)->Shininess;
		Float32 power = aqua::Float32(lFbxDouble1 * (1.0f / 1024.0f));
//		wxLogVerbose("Shininess %8.4f ", power);
		if(!((power <= 1.0f) && (power >= 0.0f))) {
			wxLogError("Material %s	 ", name.c_str());
			wxLogError("Shininess %8.4f ", lFbxDouble1);
			wxLogError("Shininess out of range");
			power = 0.99f;
		}
		material->specular.power = power;
	} else {
		wxLogError("Material %s	 ", name.c_str());
		wxLogError("Unkown type of Material %d (Possibly a shader).", materialIndex);
	}
	return;
}

enum TextureBlendMode parseFbxBlendMode(FbxLayeredTexture::EBlendMode blendMode)
{
	TextureBlendMode result;
	switch(blendMode) {
		case FbxLayeredTexture::eModulate:
			result = BLEND_MULTIPLY;
			break;
		case FbxLayeredTexture::eAdditive:
			result = BLEND_ADD;
			break;
		case FbxLayeredTexture::eOver:
			result = BLEND_REPLACE;
			break;
		default:
			result = BLEND_MULTIPLY;
			break;
	}
	return result;
}

/**
 * Fish a named generic node out of the scene.
 * Used for finding splines and other nodes that carry custom data
 * @param soughtNodeName name of node to find
 * @return pointer to generic node in associated scene or NULL
 */
FbxGenericNode* FbxParse::getNamedGenericNode(const KString& soughtNodeName)
{
	// to do - cache lookup if linear search times become unberable
	int				 genericNodeIndex;
	FbxGenericNode* node  = NULL;
//	wxLogVerbose("Sought node name %s ", soughtNodeName.Buffer());
//	wxLogVerbose("%d Generic Nodes", scene_->GetGenericNodeCount());
	for(genericNodeIndex = 0; genericNodeIndex < scene_->GetGenericNodeCount(); ++genericNodeIndex) {
		node = scene_->GetGenericNode(genericNodeIndex);
///		wxLogVerbose("Node Name %s ", node->GetName());
		if(KString(node->GetName()) == soughtNodeName) {
			break;
		}
	}
	if(genericNodeIndex < scene_->GetGenericNodeCount()) {
		return node;
	} else
		return NULL;
}


