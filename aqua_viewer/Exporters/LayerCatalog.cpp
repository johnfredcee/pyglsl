#include <StdAfx.h>

#include <PbSystemTypes.h>
#include <Utils/HashString.h>
#include <Utils/Logger.h>
#include <CRC/CRC.h>
#include <MetaData/Dynamic/DynamicMetaData.h>
#include <MetaData/Dynamic/DynamicMetaParser.h>

#include "../FbxParse.h"
#include "../SceneManager.h"
#include "../LayerManager.h"
#include "LayerCatalog.h"

bool LayerCatalog::exportLayerCatalog(const std::string& layerName, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles)  {
	// create the catalog
	DynamicMetaParser catalogParser;
	LayerManager::getLayerCatalog(layerName.c_str(), metaData, catalogParser );
	PBASSERT(catalogParser.IsValid(), "Error allocating catalog parser");
	DynamicMetaParser idsParser(catalogParser, CalcCRC("layerIds")); 
	// set id array size	
	PBASSERT(idsParser.IsValid(), "Error allocating layer catalog for layer");
	int startIndex = idsParser.GetArrayCount();
	idsParser.SetArrayCount(handles.size() + startIndex);		
	// iterate over each object in the layer
	for(aqua::Array<SceneManager::Handle>::iterator h_it = handles.begin(); h_it != handles.end() ; ++h_it) {
		// neeed these to access stuff
		const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());								
		// fish out the name of the object for the entity		
		aqua::shared_ptr<const aqua::IGraphicObject> object = sceneManager->getObject(*h_it);
		aqua::Locator::checkIn(sceneManager);
		const std::string objectName(object->getObjectName());
		const aqua::Locator::Service<LayerManager>& layerManager(aqua::Locator::checkOut<LayerManager>());				
		if (!layerManager->validForLayer(layerName, objectName)) {
			const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
			sceneManager->markAsUnexportable();
			aqua::Locator::checkIn(sceneManager);
			wxLogError("Error with object %s: shoud not be found in layer %s", objectName.c_str(), layerName.c_str());
			continue;
		}		
		idsParser.Find(startIndex + (h_it - handles.begin()), MetaParser::FIND_BY_INDEX).SetValue(CalcCRC(objectName.c_str()));		
		aqua::Locator::checkIn(layerManager);
	}
	return true;
}
