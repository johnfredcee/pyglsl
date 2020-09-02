

#include <StdAfx.h>

#include <Locator.h>
#include <PbSystemTypes.h>
#include <Utils/HashString.h>
#include <Utils/Logger.h>
#include <Utils/Array.h>
#include <CRC/CRC.h>
#include <MetaData/Dynamic/DynamicMetaData.h>
#include <MetaData/Dynamic/DynamicMetaParser.h>

#include <wx/glcanvas.h>


#include "../FbxParse.h"
#include "../SceneManager.h"
#include "../ViewerFrame.h"

#include "ILayerExporter.h"
#include "AiSplineLayerExporter.h"
#include "../LayerManager.h"

using namespace aqua;

bool AiSplineLayerExporter::ExportLayer(aqua::Uint32 componentTypeId, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles) {
	// first, retrieve the entity type
	const MetaType *componentType = metaData.FindType(componentTypeId);
	if(componentType == NULL)
		wxLogError(wxString::Format("Failed to find component %04x ", componentTypeId));
	PBASSERT(componentType != NULL, "Unknown component type");
	// Find the layer name this exporter is dealing with.
	const char* exportComponentType(metaData.FindString(componentTypeId));
	if(!exportComponentType) {
		PBBREAK("Cannot find Layer Name for object of type: " << componentTypeId);
		wxLogError(wxString::Format("Cannot find Layer Name for object of type: %04x", componentTypeId));
		const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
		sceneManager->markAsUnexportable();
		aqua::Locator::checkIn(sceneManager);
		return false;
	}
	const aqua::Locator::Service<LayerManager>& layerManager(aqua::Locator::checkOut<LayerManager>());
	const std::string layerName(layerManager->getCurrentLayerName());
	// an array to hold the id of each spline component in the layer
	std::vector<aqua::Uint32> splineComponentIds;
	// for each object in the layer
	for(aqua::Array<SceneManager::Handle>::iterator h_it = handles.begin(); h_it != handles.end() ; ++h_it) {
		// fish out the name of the object for the component
		const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
		aqua::shared_ptr<aqua::IGraphicObject> object = sceneManager->getObject(*h_it);
		aqua::Locator::checkIn(sceneManager);
		std::string componentName(object->getObjectName());
		if(!layerManager->validForLayer(layerName, componentName)) {
			const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
			sceneManager->markAsUnexportable();
			aqua::Locator::checkIn(sceneManager);
			wxLogError("Error with object %s: shoud not be found in layer %s", componentName.c_str(), layerName.c_str());
			aqua::Locator::checkIn(layerManager);
			return false;
		}
		// create an component instance
		DynamicMetaParser componentParser(metaData.AllocateData((componentName + std::string("_Track")).c_str(), componentTypeId));
		aqua::Uint32 componentId = componentParser.GetId();
		splineComponentIds.push_back(componentId);
		componentParser.Find(CalcCRC("splineId")).SetValue(componentName.c_str());
	}
	// create the catalog
	DynamicMetaParser catalogParser;
	LayerManager::getLayerCatalog(layerName.c_str(), metaData,	catalogParser);
	PBASSERT(catalogParser.IsValid(), "Error allocating catalog parser");
	// set id array size
	DynamicMetaParser idsParser(catalogParser, CalcCRC("layerIds"));
	PBASSERT(idsParser.IsValid(), "Error allocating layer catalog for layer");
	int startIndex = idsParser.GetArrayCount();
	idsParser.SetArrayCount(startIndex+splineComponentIds.size());
	// fill in the ids array
	for(std::vector<aqua::Uint32>::iterator s_it = splineComponentIds.begin(); s_it != splineComponentIds.end(); ++s_it) {
		idsParser.Find(startIndex + (s_it - splineComponentIds.begin()), MetaParser::FIND_BY_INDEX).SetValue(*s_it);
	}
	aqua::Locator::checkIn(layerManager);
	return true;
}

bool AiSplineLayerExporter::ExportDataLayer(const std::string& layerName, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles) {
	PBUNREFERENCED_PARAMETER(handles);
	PBUNREFERENCED_PARAMETER(metaData);
	PBUNREFERENCED_PARAMETER(layerName);
	return false;
};

int AiSplineLayerExporter::LayerEntityCount(aqua::Array<SceneManager::Handle>& handles) {
	// simple: there's one for every max object!
	return handles.size();
}
