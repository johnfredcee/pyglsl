#ifndef HELPEROBJECT_H
#define HELPEROBJECT_H

#include <vector>
#include <string>

#include <PbSystemTypes.h>
#include <Types.h>

class FbxParse;

namespace FBXFILESDK_NAMESPACE {
	class KFbxNode;
};

namespace aqua {

	class HelperObject : public GraphicObject {
	public:
		HelperObject(FbxParse* parser, KFbxNode* pNode, shared_ptr<DynamicMetaData> metaData);

		// metadata query
		bool ids(std::vector<crc32>& ids);

		// type query
		Rtti type() const;

	};
	
}


#endif
