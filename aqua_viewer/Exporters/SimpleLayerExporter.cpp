
#include <StdAfx.h>

#include <PbSystemTypes.h>
#include <Utils/HashString.h>
#include <Utils/Logger.h>
#include <CRC/CRC.h>
#include <MetaData/Dynamic/DynamicMetaData.h>
#include <MetaData/Dynamic/DynamicMetaParser.h>

#include <wx/glcanvas.h>


#include "../FbxParse.h"
#include "../SceneManager.h"

#include "ILayerExporter.h"
#include "SimpleLayerExporter.h"
#include "../LayerManager.h"
#include "LayerCatalog.h"
using namespace aqua;

/**
 * This is a set of export rules for a "Simple Layer".	Every max
 * object in the layer is an instance of the enitytTypeId passed to
 * the Layer exporter The entity is layed out in meta headers in a
 * special way - eg an Oil Barrel
 * struct OilBarrelEntityType {
 *	 MetaEntityReference self;
 *	 MetaEntityReference parent;
 *	 MetaComponentReference SpatialComponentData;
 *	 MetaComponentReference GraphicModelComponentData;
 * };
 * The components it refers to are described by
 * MetaComponentReference. The variable name field of the struct
 * actually is the Type name of the component it refers to. In this
 * case a OilBarrelEntityType is a SpatialComponentData +
 * GraphicModelComponet data component. The components are resolved
 * and filled in automatically by this magic meta code. This actually
 * handles the majority of cases of things we want to export.
 **/

bool SimpleLayerExporter::ExportLayer(aqua::Uint32 entityTypeId, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles)
{
	std::vector<Uint32> entityIds;
	// first, retrieve the entity type
	const MetaType *entityType = metaData.FindType(entityTypeId);
	PBASSERT(entityType != NULL, "Unknown entity type");
	if(entityType == NULL) {
		const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
		sceneManager->markAsUnexportable();
		aqua::Locator::checkIn(sceneManager);
		wxLogError(wxString::Format("Failed to find entity %04x ", entityTypeId));
		return false;
	}
	// Find the layer name this exporter is dealing with.
	const char* exportEntityType(metaData.FindString(entityTypeId));
	if(!exportEntityType) {
		PBBREAK("Cannot find Layer Name for object of type: " << entityTypeId);
		wxLogError(wxString::Format("Cannot find Layer Name for object of type: %04x", entityTypeId));
		const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
		sceneManager->markAsUnexportable();
		aqua::Locator::checkIn(sceneManager);
		return false;
	}
	// get the layer
	const aqua::Locator::Service<LayerManager>& layerManager(aqua::Locator::checkOut<LayerManager>());
	const std::string layerName(layerManager->getCurrentLayerName());
	// for each object in the layer
	for(aqua::Array<SceneManager::Handle>::iterator h_it = handles.begin(); h_it != handles.end() ; ++h_it) {
		// neeed these to access stuff
		const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
		// fish out the name of the object for the entity
		aqua::shared_ptr<const aqua::IGraphicObject> object = sceneManager->getObject(*h_it);
		aqua::Locator::checkIn(sceneManager);
		const std::string entityName(object->getObjectName());
		const std::string entityTypeName(object->type().getName());
		aqua::HashString parentId(object->getParent());
		// check this entiy should be found in this layer.
		if(!layerManager->validForLayer(layerName, entityName)) {
			const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
			sceneManager->markAsUnexportable();
			aqua::Locator::checkIn(sceneManager);
			wxLogError("Error with entity %s: shoud not be found in layer %s", entityName.c_str(), layerName.c_str());
			aqua::Locator::checkIn(layerManager);
			return false;
		}
		// create an entity instance
		DynamicMetaParser entityParser(metaData.AllocateData(entityName.c_str(), entityTypeId));
		aqua::Uint32 entityId = entityParser.GetId();
		// check it's valid (don't trust meta much)
		DynamicMetaParser::Iterator entityIterator(entityParser);
		PBASSERT(entityIterator.IsValid(), "Error with Entity: " << entityName);
		if(!entityIterator.IsValid()) {
			const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
			sceneManager->markAsUnexportable();
			aqua::Locator::checkIn(sceneManager);
			wxLogError("Error with Entity: '%s' of Type: '%s' in Layer: '%s'", entityName.c_str(), entityTypeName.c_str(), layerName.c_str());
			wxLogError("This could be a duplicate object, or something in the wrong layer!");
			aqua::Locator::checkIn(layerManager);
			return false;
		}
		// keep track of ids we have exported
		entityIds.push_back(entityId);
		// check the first item (field in struct) in the entity type is a MetaEntityReference
		PBASSERT(entityIterator.GetTypeId() == CalcCRC("MetaEntityReference"), "self member is wrong type");
		if(entityIterator.GetTypeId() != CalcCRC("MetaEntityReference")) {
			const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
			sceneManager->markAsUnexportable();
			aqua::Locator::checkIn(sceneManager);
			wxLogError("self member of %s is wrong type", entityIterator.GetTypeName());
			aqua::Locator::checkIn(layerManager);
			return false;
		}
		entityIterator.Find("entityId").SetValue(entityId);	   // set the "self"
		entityIterator.Next();								   // skip to the "parent"
		PBASSERT(entityIterator.GetTypeId() == CalcCRC("MetaEntityReference"), "parent member is wrong type");
		if(entityIterator.GetTypeId() != CalcCRC("MetaEntityReference")) {
			const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
			sceneManager->markAsUnexportable();
			aqua::Locator::checkIn(sceneManager);
			wxLogError("parent member of %s is wrong type", entityIterator.GetTypeName());
			aqua::Locator::checkIn(layerManager);
			return false;
		}
		entityIterator.Find("entityId").SetValue(parentId);
		entityIterator.Next();
		// now do the components : the rest of the items are in the form (MetaComponentReference ComponentType)
		// where ComponentType names an actual component we are going to create allocate and fill
		std::vector<wxString> componentTypeNames;
		std::vector<wxString> componentNames;
		// TO DO : component Name should include a number
		while(entityIterator.IsValid()) {
			PBASSERT(entityIterator.GetTypeId() == CalcCRC("MetaComponentReference"), "Component is not correct type");
			if(entityIterator.GetTypeId() != CalcCRC("MetaComponentReference")) {
				const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
				sceneManager->markAsUnexportable();
				aqua::Locator::checkIn(sceneManager);
				wxLogError("Component is not correct type");
				aqua::Locator::checkIn(layerManager);
				return false;
			}
			// collect the name and type of each component
			wxLogVerbose(wxString::Format("entityElementIterator : element	%s %s",	 wxString(entityIterator.GetTypeName(), wxConvUTF8).c_str(), wxString(entityIterator.GetName(), wxConvUTF8).c_str()));
			wxString componentName(wxString::Format("Component_%04x_%04x", entityId, CalcCRC(entityIterator.GetName())));
			componentNames.push_back(componentName);
			wxString componentTypeName(entityIterator.GetName());
			componentTypeNames.push_back(componentTypeName);
			entityIterator.Find("id").SetValue(CalcCRC(componentName.c_str()));
			entityIterator.Next();
		}
		// for each component in the entity, allocate an instance of that component and fill it in
		std::vector<wxString>::iterator it_t = componentTypeNames.begin();
		for(std::vector<wxString>::iterator it = componentNames.begin(); it != componentNames.end(); ++it) {
			metaData.AllocateData(it->c_str(), CalcCRC(it_t->c_str()));
			object->initialiseComponent(metaData, std::string(it->c_str()), CalcCRC(it_t->c_str()));
			++it_t;
		}
	} // for each object in the layer
	aqua::Locator::checkIn(layerManager);
	DynamicMetaParser catalogParser;
	LayerManager::getLayerCatalog(layerName, metaData, catalogParser);
	PBASSERT(catalogParser.IsValid(), "Error allocating layer catalog for layer");
	// set id array size
	DynamicMetaParser idsParser(catalogParser, CalcCRC("layerIds"));
	PBASSERT(idsParser.IsValid(), "Error allocating layer catalog for layer: " << layerName);
	int startIndex = idsParser.GetArrayCount();
	idsParser.SetArrayCount(entityIds.size() + startIndex);
	// iterate over ids and set array elements
	for(std::vector<aqua::Uint32>::iterator e_it = entityIds.begin(); e_it != entityIds.end(); ++e_it) {
		idsParser.Find(startIndex + (e_it - entityIds.begin()), MetaParser::FIND_BY_INDEX).SetValue(*e_it);
	}
	return true;
}

int SimpleLayerExporter::LayerEntityCount(aqua::Array<SceneManager::Handle>& handles)
{
	// simple: there's one for every max object!
	return handles.size();
}

bool SimpleLayerExporter::ExportDataLayer(const std::string& layerName, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles)
{
	return LayerCatalog::exportLayerCatalog(layerName, metaData, handles);
}
