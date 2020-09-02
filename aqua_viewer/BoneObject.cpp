
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
#include "BoneObject.h"

using namespace aqua;

BoneObject::BoneObject(FbxParse* parser, KFbxNode* pNode, shared_ptr<DynamicMetaData> metaData) {
	PBUNREFERENCED_PARAMETER(parser);
	// common parts
	initialise(pNode);
}

bool BoneObject::ids(std::vector<crc32>& ids) {
	return !ids.empty();
}

Rtti BoneObject::type() const {
	return Rtti("BoneObject");
};

