#include "StdAfx.h"

#include <MetaData/Dynamic/DynamicMetaData.h>
#include <MetaData/Dynamic/DynamicMetaParser.h>
#include <Mathematics/PbMatrix.h>


#include "Rtti.h"
#include "FbxParse.h"
#include "IntermediateMesh.h"
#include "IMesh.h"
#include "IGraphicObject.h"
#include "SceneManager.h"
#include "GraphicObject.h"
#include "HelperObject.h"


using namespace aqua;

HelperObject::HelperObject(FbxParse* parser, KFbxNode* pNode, shared_ptr<DynamicMetaData> metaData) {

	PBUNREFERENCED_PARAMETER(parser);
	initialise(pNode);
	
	const aqua::Locator::Service<SceneManager> &sceneManager(aqua::Locator::checkOut<SceneManager>()); 
	std::string nodeName(pNode->GetName());
	std::string helperTypeName("HelperData");
	DynamicMetaParser helperParser = sceneManager->allocateData(getObjectName().c_str(), helperTypeName);
	PBASSERT(helperParser.IsValid(), "Failed to allocate helper");
	DynamicMetaParser positionParser = helperParser.Find(CalcCRC("position"));
	PBASSERT(helperParser.IsValid(), "Failed to find position");
	PbReal x,y,z;
	getPosition(x,y,z);
	positionParser.Find(CalcCRC("x")).SetValue(x);
	positionParser.Find(CalcCRC("y")).SetValue(y);
	positionParser.Find(CalcCRC("z")).SetValue(z);
	PbMatrix3 rotMatrix;
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			rotMatrix(i,j) = (*transformMatrix_)(i, j);
	PbVector3 axis;
	// Float32 angle;
	// hope there isn't any scale
	rotMatrix.orthonormalize();
	PbQuaternion q(rotMatrix);
	DynamicMetaParser rotationParser = helperParser.Find(CalcCRC("orientation"));
	rotationParser.Find(CalcCRC("x")).SetValue(q.x());
	rotationParser.Find(CalcCRC("y")).SetValue(q.y());
	rotationParser.Find(CalcCRC("z")).SetValue(q.z());
	rotationParser.Find(CalcCRC("w")).SetValue(q.w());	
	aqua::Locator::checkIn(sceneManager);
	return;
}

bool HelperObject::ids(std::vector<crc32>& ids) {
	ids.push_back(CalcCRC(getObjectName().c_str()));
	return !ids.empty();
}

Rtti HelperObject::type() const {
	return Rtti("HelperObject");
};

