#ifndef SPLINEOBJECT_H
#define SPLINEOBJECT_H

class FbxParse;
class DynamicMetaData;

namespace FBXSDK_NAMESPACE {
	class FbxNode;
}

namespace aqua {
	class SplineObject : public GraphicObject {
	public:
		SplineObject(FbxParse* parser, FbxNode* pNode, shared_ptr<DynamicMetaData> metaData);

		// number of knots		
		int knotCount() const;

		// getKnot
		PbVector3 getKnot(int i, bool transformed = false) const;

		// addKnot
		int addKnot(const PbVector3& knot);

		// metadata query
		bool ids(std::vector<crc32>& ids);

		// type query
		Rtti type() const;

	private:
		// atual knot points
		aqua::Array<aqua::PbVector3> knots_;
	};	
};

#endif
