
#include "StdAfx.h"

#include "MetaDataManager.h"

/**
 * Create a new metadata object with a given name
 * @param name Name of object
 * @return shared pointer to object
 */
aqua::shared_ptr<DynamicMetaData> MetaDataManager::newMetaDataObject(std::string name) {
	aqua::shared_ptr<DynamicMetaData> result;
	if (metaObjects_.find(name) == metaObjects_.end()) {
		result = aqua::shared_ptr<DynamicMetaData>(new DynamicMetaData);
		metaObjects_[name] = result;
	} 
	return result;
}

/**
 * Extract named metadata object
 * @param name Name of object
 * @return const pointer to object or null if no object
 */
aqua::shared_ptr<DynamicMetaData>  MetaDataManager::getMetaDataObject(std::string name) {
	aqua::shared_ptr<DynamicMetaData> result;
	MetaDataDictionary::iterator it = metaObjects_.find(name);
	result = (it == metaObjects_.end()) ? aqua::shared_ptr<DynamicMetaData>() : it->second;
	return result;			   
}

/**
 * Delete the metadata object with the given name
 * @param name Name of object to delete
 */
void MetaDataManager::deleteMetaDataObject(std::string name) {
	MetaDataDictionary::iterator it = metaObjects_.find(name);
	if (it != metaObjects_.end())
		metaObjects_.erase(it);
	return;	
}
