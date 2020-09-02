#include "StdAfx.h"

#include <wx/glcanvas.h>

#include "Viewer.h"
#include "ViewerFrame.h"
#include "SceneManager.h"
#include "Exporters/ILayerExporter.h"
#include "Exporters/ILayerExporter.h"
#include "Exporters/SimpleLayerExporter.h"
#include "Exporters/ConeLayerExporter.h"
#include "Exporters/TyreLayerExporter.h"
#include "Exporters/FurnitureLayerExporter.h"
#include "Exporters/AiSplineLayerExporter.h"
#include "Exporters/CarLayerExporter.h"
#include "Exporters/StaticPhysicsLayerExporter.h"
#include "Exporters/OilBarrelLayerExporter.h"
#include "Exporters/WaterBarrelLayerExporter.h"
#include "Exporters/SkinnedLayerExporter.h"
#include "Exporters/DataLayerExporter.h"
#include "Exporters/ActiveLayerExporter.h"

#include "LayerManager.h"


/**
 * If you want to add a new layer, do so at initialisation time in ViewerFrame.cpp
 */
// TO DO -- maybe this should take layerExporter as a parameter
bool LayerManager::addLayer(const std::string& name, const std::string& entityName, aqua::shared_ptr<ILayerExporter> exporter, const std::string& regexp)
{
	aqua::shared_ptr<Layer> newLayer(new Layer(entityName, exporter, regexp));
	layers_[name] = newLayer;
	return true;
}

/**
 * Return the name of the layer currently being processed
 * @return Name of the layer
 */
std::string LayerManager::getCurrentLayerName()
{
	return currentLayerName_;
}

/**
 * Test if a name is of a form that matches the regexp filter for the given layer
 * @param layerName Layer to validate name for
 * @param name Name to validate
 * @return True if name is valud
 */
bool LayerManager::validForLayer(const std::string& layerName, const std::string& name)
{
	bool result = true;
	LayerDictionary::iterator l_it = layers_.find(getLayerName(layerName));
	if(l_it != layers_.end()) {
		if(l_it->second->regex_.IsValid()) {
			result = l_it->second->regex_.Matches(name.c_str());
		}
	}
	return result;
}

/**
 * Gvien a metadata file and a layer name, invoke the right exporter for that layer.
 * @param name Name of layer to export
 * @param md Metadata object to export to
 * @return true if successful
 */
bool LayerManager::exportLayer(const std::string& name)
{
	bool result = false;
	
	// TO DO -- check layer doesn't already exist, strip group name off
	LayerDictionary::iterator l_it = layers_.find(getLayerName(name));
	if(l_it == layers_.end()) {
		wxLogVerbose("Failed to find exporter for layer %s -- no special processing.", name.c_str());
		return result;
	} 
	wxLogVerbose("Exporting layer %s ", name.c_str());	
	aqua::Array<SceneManager::Handle> handles;
	const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
	sceneManager->getSceneObjectsInLayer(name, handles);
	currentLayerName_ = getLayerName(name);
	// if we found an associated exporter 
	if(l_it->second->exporter_) {
		// if it doesn't have a method, fall back to the data layer exporter (just exports a layer catalog)
		if(l_it->second->entity_ == std::string("DataLayer")) {			
			result = l_it->second->exporter_->ExportDataLayer(currentLayerName_, *(sceneManager->metaData_), handles);
		} else {
			// if there is an entity associated with this layer, export the specific entity data
			aqua::Uint32 entityId = CalcCRC(l_it->second->entity_.c_str());
			result = l_it->second->exporter_->ExportLayer(entityId, *(sceneManager->metaData_), handles);
			// export layer catalog as well
			if (result) {
				result = l_it->second->exporter_->ExportDataLayer(currentLayerName_, *(sceneManager->metaData_), handles);			
			}
		}
	}
	aqua::Locator::checkIn(sceneManager);
	return result;
}

/**
 * Get the layer name portion of a composite layer#group name
 * @param name Full name
 * @return Layer name portion
 */
std::string LayerManager::getLayerName(const std::string& name)
{
	size_t pos = name.find_first_of('#');
	return (pos == std::string::npos) ? name : name.substr(0, pos);
}

/**
 * Return the name of the layer we expect to see this entity inside
 * @param entityType type of entity to use in layer lookup
 * @return layer the entity is expected to be inside
 */

const std::string LayerManager::getLayerNameFromEntityType(const std::string& entityType) const
{
	for(LayerDictionary::const_iterator layer(layers_.begin()); layer != layers_.end(); ++layer) {
		if(entityType == layer->second->entity_)
			return layer->first;
	}
	return "Unknown Layer";
}

/**
 * Quick and dirty factory object to construct an exporter functor
 * object given the method of the layer it exports from
 * @param method method of layer
 * @return shared_ptr to exporter functor
 */
aqua::shared_ptr<ILayerExporter> LayerManager::newExporterObject(const wxString& method)   //
{
	/* yes, the moral equivalent of a switch statement ! */	
	if(method == "ConeLayerExporter") {
		return aqua::shared_ptr<ILayerExporter>(new ConeLayerExporter());
	};
	if(method == "TyreLayerExporter") {
		return aqua::shared_ptr<ILayerExporter>(new TyreLayerExporter());
	};
	if(method == "AiSplineLayerExporter") {
		return aqua::shared_ptr<ILayerExporter>(new AiSplineLayerExporter());
	};
	if(method == "FurnitureLayerExporter") {
		return aqua::shared_ptr<ILayerExporter>(new FurnitureLayerExporter());
	};
	if(method == "WaterBarrelLayerExporter") {
		return aqua::shared_ptr<ILayerExporter>(new WaterBarrelLayerExporter());
	};
	if(method == "OilBarrelLayerExporter") {
		return aqua::shared_ptr<ILayerExporter>(new OilBarrelLayerExporter());
	};
	if(method == "CarLayerExporter") {
		return aqua::shared_ptr<ILayerExporter>(new CarLayerExporter());
	}
	if(method == "StaticPhysicsLayerExporter") {
		return aqua::shared_ptr<ILayerExporter>(new StaticPhysicsLayerExporter());
	};
	if(method== "AnimationLayerExporter") {
		return aqua::shared_ptr<ILayerExporter>(new SkinnedLayerExporter());		
	}
	if(method == "ActiveLayerExporter") {
		return aqua::shared_ptr<ILayerExporter>(new ActiveLayerExporter());		
	}
	// a fallback layer for which there is no known entity
	if(method == "DataLayerExporter") {
		return aqua::shared_ptr<ILayerExporter>(new DataLayerExporter());
	}
	return aqua::shared_ptr<ILayerExporter>();
}

/**
 * Create a layer catalog object for a given layer. A layer catalog is
 * a simple structure containing an array of ids of all the data in
 * that layer
 *
 * @param layerName Name of layer to export
 * @param metaData Metadata object of layer
 * @param catalogParser 
 */
void LayerManager::getLayerCatalog(const std::string& layerName, DynamicMetaData& metaData, DynamicMetaParser& catalogParser)
{
	catalogParser = DynamicMetaParser(metaData, CalcCRC(layerName.c_str()));
	if(!catalogParser.IsValid()) {
		// we need a layer catalog named after the layer with the id of each layer in it
		catalogParser = metaData.AllocateData(layerName.c_str(),  CalcCRC("LayerCatalog"));
		PBASSERT(catalogParser.IsValid(), "Error allocating catalog parser");
	}
	return;
}
