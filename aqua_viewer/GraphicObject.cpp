#include "StdAfx.h"

#include <CRC/CRC.h>
#include <MetaData/Dynamic/DynamicMetaData.h>
#include <MetaData/Dynamic/DynamicMetaParser.h>


#include "Rtti.h"
#include "FbxParse.h"
#include "IntermediateMesh.h"
#include "IMesh.h"
#include "IGraphicObject.h"
#include "GraphicObject.h"
#include "SplineObject.h"
#include "HelperObject.h"
#include "MeshObject.h"
#include "BoneObject.h"
#include "MetaSerializer.h"
#include "SceneManager.h"

extern KFbxSdkManager* fbxSDKManager;

using namespace aqua;

shared_ptr< IGraphicObject > GraphicObject::create(FbxParse* parser, FbxNode* node, shared_ptr<DynamicMetaData> metaData) {

	shared_ptr< IGraphicObject > result;

	//KFbxNode *root = parser->getSceneRoot();
	KFbxNodeAttribute::EType lAttributeType = (node->GetNodeAttribute()) ? (node->GetNodeAttribute()->GetAttributeType()) : KFbxNodeAttribute::eNull;
	switch (lAttributeType)	{
		case KFbxNodeAttribute::eMesh:	{
			result = shared_ptr< IGraphicObject >( new MeshObject(parser, node, metaData) );
			break;
		}
		case KFbxNodeAttribute::eSkeleton: {
			result = shared_ptr< IGraphicObject >( new BoneObject(parser, node, metaData) );
			break;			
		}
		case KFbxNodeAttribute::eNull: {
			KFbxProperty isSplineProperty(node->FindProperty("IsSpline"));
			if (isSplineProperty.IsValid()) {
				result = shared_ptr< IGraphicObject >( new SplineObject(parser, node, metaData) );
				break;
			} else {
				result = shared_ptr< IGraphicObject >( new HelperObject(parser, node, metaData) );
				break;
			}
			break;
		}			
		default:
			result = shared_ptr< IGraphicObject >( new GraphicObject() );
			boost::dynamic_pointer_cast< GraphicObject >( result )->initialise(node) ;
			break;
	}
	return result;
}

void GraphicObject::initialise(FbxNode *pNode)
{
	std::string nodeName(pNode->GetName());
	FbxNode*   parent(pNode->GetParent());
	std::string parentName((pNode->GetParent() != NULL) ? std::string("root") : std::string(parent->GetName()));

	fbxNode_       = pNode;
	objectName_    = nodeName;
	parentName_    = parentName;
	name_          = aqua::HashString(nodeName.c_str());
	parent_        = aqua::HashString(parentName.c_str());
	const cyan::Locator::Service<SceneManager> &sceneManager(cyan::Locator::checkOut<SceneManager>()); 
	// compute the transformation of the node in the scene
	transformMatrix_ = sceneManager->computeSceneTransform(pNode);
	relativeTransformMatrix_ = sceneManager->computeRelativeSceneTransform(pNode);
	parentTransformMatrix_ = sceneManager->computeSceneTransform(parent);
	aqua::Locator::checkIn(sceneManager);	
}

shared_ptr < const PbMatrix4 > GraphicObject::getTransformMatrixPtr() const {
	return transformMatrix_;
}

shared_ptr< PbMatrix4 > GraphicObject::getTransformMatrixPtr()	{
	return transformMatrix_;
}

shared_ptr < const PbMatrix4 > GraphicObject::getRelativeTransformMatrixPtr() const {
	return relativeTransformMatrix_;
}

shared_ptr < const PbMatrix4 > GraphicObject::getParentTransformMatrixPtr() const {
	return parentTransformMatrix_;
}

PbMatrix4 GraphicObject::transform() {
	return *transformMatrix_;
}


void GraphicObject::getPosition(PbReal& x, PbReal& y, PbReal& z) const {
	PbVector4 tlt = transformMatrix_->getColumn(3);
	x = tlt.x();
	y = tlt.y();
	z = tlt.z();
	return;
}

void GraphicObject::setPosition(PbReal x, PbReal y, PbReal z) {
	transformMatrix_->setColumn(3, PbVector4(x, y, z, 1.0));
}

//! return the minimum extent of the bounding box
const PbVector3 GraphicObject::getMinExtent() const
{
	// default extent is only a point
	PbVector3 result;
	getPosition(result.x(), result.y(), result.z());
	return result;
}

//! return the maximum extent of the bounding box
const PbVector3 GraphicObject::getMaxExtent() const
{
	PbVector3 result;
	getPosition(result.x(), result.y(), result.z());
	return result;	
}

const HashString& GraphicObject::getName() const {
	return name_;
}

const std::string& GraphicObject::getObjectName() const {
	return objectName_;
}

const std::string& GraphicObject::getHint() const {
	return hint_;
}
	
void GraphicObject::setObjectName(const std::string& objectName) {
	size_t pos = objectName.find_first_of('~');		 
	objectName_ = ( pos == std::string::npos ) ? objectName : objectName.substr(0, pos);
	name_ = HashString( objectName.c_str() );
	hint_ = ( pos == std::string::npos ) ? "default" : objectName.substr(pos+1);
}
	
   
HashString GraphicObject::getParent() const {
	return parent_;
}


const std::string& GraphicObject::getParentName(void) const
{
	return parentName_;
}

void GraphicObject::addChild(HashString child) {
	children_.pushBack(child);
}

const HashString& GraphicObject::getChild(int n) const {
	return children_.at(n);
}

int GraphicObject::nChildren() const {
	return children_.size();
}


void GraphicObject::serialiseSelf(DynamicMetaData &) const {
	
}


void GraphicObject::deserialiseSelf(DynamicMetaData &) {
	
}

Rtti GraphicObject::type(void) const {
	return Rtti("Graphic");
}


bool GraphicObject::initialiseComponent(DynamicMetaData &metaData,const std::string &componentName, aqua::Uint32 componentTypeId) const {
	
	bool result = false;
		
	DynamicMetaParser componentParser(metaData, CalcCRC(componentName.c_str()));
	PBASSERT(componentParser.IsValid(), "Unable to find component");
	// yes, i know - but no time for something more elegant and polymorphic
	if (componentTypeId == CalcCRC("SpatialComponentData")) {
		DynamicMetaParser positionParser(componentParser.Find("position"));
		const aqua::Float32 *transform = getTransformMatrixPtr()->data();			
		PBASSERT(positionParser.IsValid(), "SpatialComponentData should have a position field!");				
		PbReal x,y,z;				
		getPosition(x,y,z);
		DynamicMetaParser xParser(positionParser.Find("x"));
		PBASSERT(xParser.IsValid(), "No x in PbVector3");
		xParser.SetValue(transform[3]);
		DynamicMetaParser yParser(positionParser.Find("y"));
		PBASSERT(yParser.IsValid(), "No y in PbVector3");
		yParser.SetValue(transform[7]);
		DynamicMetaParser zParser(positionParser.Find("z"));
		PBASSERT(zParser.IsValid(), "No z in PbVector3");
		zParser.SetValue(transform[11]);
		DynamicMetaParser rotationParser(componentParser.Find("rotation"));
		PBASSERT(rotationParser.IsValid(), "SpatialComponentData should have a rotation field!");
		DynamicMetaParser matrixParser(rotationParser.Find("mat"));
		PBASSERT(matrixParser.IsValid(), "SpatialComponentData should have a rotation field!");				
		matrixParser.Find(0, MetaParser::FIND_BY_INDEX).SetValue(transform[0]);
		matrixParser.Find(1, MetaParser::FIND_BY_INDEX).SetValue(transform[1]);
		matrixParser.Find(2, MetaParser::FIND_BY_INDEX).SetValue(transform[2]);								
		matrixParser.Find(3, MetaParser::FIND_BY_INDEX).SetValue(transform[4]);
		matrixParser.Find(4, MetaParser::FIND_BY_INDEX).SetValue(transform[5]);
		matrixParser.Find(5, MetaParser::FIND_BY_INDEX).SetValue(transform[6]);								
		matrixParser.Find(6, MetaParser::FIND_BY_INDEX).SetValue(transform[8]);
		matrixParser.Find(7, MetaParser::FIND_BY_INDEX).SetValue(transform[9]);
		matrixParser.Find(8, MetaParser::FIND_BY_INDEX).SetValue(transform[10]);
		matrixParser.Find(12, MetaParser::FIND_BY_INDEX).SetValue(0.0f);
		matrixParser.Find(13, MetaParser::FIND_BY_INDEX).SetValue(0.0f);
		matrixParser.Find(14, MetaParser::FIND_BY_INDEX).SetValue(0.0f);
		matrixParser.Find(15, MetaParser::FIND_BY_INDEX).SetValue(1.0f);
		result = true;
	} 
	return false;
}

KFbxNode* GraphicObject::getFbxNode() {
	return fbxNode_;
}

bool GraphicObject::ids(std::vector<crc32>& ids) {
	return !ids.empty();
}

void GraphicObject::render() const {
	
}
