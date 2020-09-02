
#include "StdAfx.h"
#include <limits>
#include "Rtti.h"
#include "ViewerFrame.h"
#include "FbxParse.h"
#include "IntermediateMesh.h"
#include "Imesh.h"
#include "SceneManager.h"
#include "GraphicObject.h"
#include "SplineObject.h"

using namespace aqua;

SplineObject::SplineObject(FbxParse* parser, FbxNode* pNode, shared_ptr<DynamicMetaData> metaData) {
	// common stuff
	initialise(pNode);
	// work out what layer we are in
	FbxDisplayLayer *layer = parser->getDisplayLayer(pNode);
	std::string nodeName(pNode->GetName());
	wxLogVerbose("Spline Node %s In Display Layer %s ", wxString(pNode->GetName(), wxConvUTF8).c_str(), wxString(layer->GetName(), wxConvUTF8).c_str());
	// spline knot data is held in a generic node with the same name as the transform node
	FbxGenericNode *splineObject = parser->getNamedGenericNode(pNode->GetName());
	PBASSERT(splineObject != NULL, "Failed to find associated custom spline data");
	FbxProperty prop(splineObject->GetFirstProperty());
	while (prop.IsValid()) {
		prop = splineObject->GetNextProperty(prop);
	}
	// for now we assume the first spline found is "the" spline
	if (splineObject) {
		int knotCount = 0;
		FbxPropertyT<int> knotCountProperty(splineObject->FindProperty("KnotCount"));
		if (knotCountProperty.IsValid()) {
			knotCount = knotCountProperty.Get();
		}
		bool isClosed = false;
		FbxPropertyT<bool> isClosedProperty(splineObject->FindProperty("IsClosed"));
		if (isClosedProperty.IsValid()) {
			isClosed = isClosedProperty.Get();
		}
		KString PropertyName;
		for (int j = 0; j < knotCount; ++j) {
			fbxDouble3 knot;
			PropertyName = "Vertex" + KString(j);
			FbxPropertyT<fbxDouble3>  vertexProperty(splineObject->FindProperty(PropertyName));
			if (vertexProperty.IsValid()) {
				knot = vertexProperty.Get();
			}
			addKnot(PbVector3(aqua::Float32(knot[0]),
							  aqua::Float32(knot[1]),
							  aqua::Float32(knot[2])));
		}
		if (isClosed) {
			addKnot(getKnot(0));
		}
	}
	const aqua::Locator::Service<SceneManager> &sceneManager(aqua::Locator::checkOut<SceneManager>());
	std::string splineTypeName("LinearSplineData");
	DynamicMetaParser splineParser = sceneManager->allocateData(getObjectName(), splineTypeName);
	DynamicMetaParser knotParser = splineParser.Find(CalcCRC("knots"));
	knotParser.SetArrayCount(knots_.size());
	for(int knotIndex = 0; knotIndex < int(knots_.size()); ++knotIndex) {
		DynamicMetaParser knotElementParser = knotParser.Find(knotIndex, MetaParser::FIND_BY_INDEX);
		knotElementParser.Find(CalcCRC("x")).SetValue(knots_[knotIndex].x());
		knotElementParser.Find(CalcCRC("y")).SetValue(knots_[knotIndex].y());
		knotElementParser.Find(CalcCRC("z")).SetValue(knots_[knotIndex].z());
	}
	// compute the transformation of the node in the scene
	transformMatrix_ = sceneManager->computeSceneTransform(pNode);
	aqua::Locator::checkIn(sceneManager);
}


int SplineObject::knotCount() const {
	return knots_.size();
}

PbVector3 SplineObject::getKnot(int i, bool transformed) const {
	PbVector3 result;
	if (transformed) {
		PbVector4 knot(knots_[i].x(), knots_[i].y(), knots_[i].z(), 1.0f);
		knot = (*transformMatrix_) * knot;
		result = PbVector3(knot.x(), knot.y(), knot.z());
	} else
		result = knots_[i];
	return result;
}

int SplineObject::addKnot(const PbVector3& knot) {
	knots_.pushBack(knot);
	return knots_.size() - 1;
}

bool SplineObject::ids(std::vector<crc32>& ids) {
	ids.push_back(CalcCRC(getObjectName().c_str()));
	return !ids.empty();
}

Rtti SplineObject::type() const {
	return Rtti("SplineObject");
};

