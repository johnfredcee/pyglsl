#ifndef METADATAMANAGER_H
#define METADATAMANAGER_H

#include "Locator.h"

#include <map>

/** Eventually we may support multiple "scenes" - ie metadata files
    This class is a stop against this eventuality */
class MetaDataManager : public aqua::Locator::ServiceSingleton<MetaDataManager> {
	
	friend class aqua::Locator::ServiceSingleton<MetaDataManager>;
public:
	typedef std::map< std::string, aqua::shared_ptr<DynamicMetaData> > MetaDataDictionary;
private:
	MetaDataDictionary metaObjects_;
public:
	aqua::shared_ptr<DynamicMetaData> newMetaDataObject(std::string name);
	aqua::shared_ptr<DynamicMetaData> getMetaDataObject(std::string name);
	void deleteMetaDataObject(std::string name);
  
};
#endif
