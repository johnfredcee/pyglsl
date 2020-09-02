#include <StdAfx.h>

#include <PbSystemTypes.h>
#include <Locator.h>
#include <Utils/HashString.h>
#include <Utils/Logger.h>
#include <CRC/CRC.h>
#include <MetaData/Dynamic/DynamicMetaData.h>
#include <MetaData/Dynamic/DynamicMetaParser.h>

#include <wx/glcanvas.h>

#include "..\FbxParse.h"
#include "..\SceneManager.h"

#include "ILayerExporter.h"
#include "CarLayerExporter.h"
#include "..\LayerManager.h"

using namespace aqua;

aqua::Uint32 CarLayerExporter::AllocateWheel(int wheelIndex, DynamicMetaData& metaData, Uint32 entityId, std::string& entityName, aqua::shared_ptr<const IGraphicObject>& wheel) {
	// allocate the actual wheel entity
	std::string wheelEntityName(entityName + "_" + wheel->getObjectName());
	DynamicMetaParser wheelEntityParser = metaData.AllocateData(wheelEntityName.c_str(), CalcCRC("WheelEntityType"));
	PBASSERT(wheelEntityParser.IsValid(), "Something wrong with wheels. Check names?");
	// walk over the nwely created wheel enitty and fill in parent and self info
	aqua::Uint32 wheelEntityId = wheelEntityParser.GetId();
	DynamicMetaParser::Iterator wheelEntityIterator(wheelEntityParser);
	PBASSERT(wheelEntityIterator.IsValid(), "Could not parse entity");
	PBASSERT(wheelEntityIterator.GetTypeId() == CalcCRC("MetaEntityReference"), "self member is wrong type");
	wheelEntityIterator.Find("entityId").SetValue(wheelEntityId);	 // set the "self"
	wheelEntityIterator.Next();
	wheelEntityIterator.Find("entityId").SetValue(entityId);		 // set the "parent"
	PBASSERT(wheelEntityIterator.GetTypeId() == CalcCRC("MetaEntityReference"), "parent member is wrong type");
	wheelEntityIterator.Next(); // skip the physics
	wheelEntityIterator.Next();
	// now set the ids within the components
	std::vector<wxString> componentTypeNames;
	std::vector<wxString> componentNames;
	while(wheelEntityIterator.IsValid()) {
		PBASSERT((wheelEntityIterator.GetTypeId() == CalcCRC("MetaComponentReference") || wheelEntityIterator.GetTypeId() == CalcCRC("MetaEntityReference")), "Entity Meta Structure element is not correct type");
		wxLogVerbose(wxString::Format("entityElementIterator : element	%s %s",	 wxString(wheelEntityIterator.GetTypeName(), wxConvUTF8).c_str(), wxString(wheelEntityIterator.GetName(), wxConvUTF8).c_str()));
		if(wheelEntityIterator.GetTypeId() == CalcCRC("MetaComponentReference")) {
			wxString componentName(wxString::Format("Component_%04x_%04x_%02x", entityId, CalcCRC(wheelEntityIterator.GetName()), wheelIndex));
			componentNames.push_back(componentName);
			wxString componentTypeName(wheelEntityIterator.GetName());
			componentTypeNames.push_back(componentTypeName);
			wheelEntityIterator.Find("id").SetValue(CalcCRC(componentName.c_str()));
			wheelEntityIterator.Next();
		}
	}
	// now allocate the components
	std::vector<wxString>::iterator it_t = componentTypeNames.begin();
	for(std::vector<wxString>::iterator it = componentNames.begin(); it != componentNames.end(); ++it) {
		metaData.AllocateData(it->c_str(), CalcCRC(it_t->c_str()));
		wheel->initialiseComponent(metaData, std::string(it->c_str()), CalcCRC(it_t->c_str()));
		++it_t;
	}
	return wheelEntityId;
}

bool CarLayerExporter::ExportLayer(aqua::Uint32 entityTypeId, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles)  {
	// first, retrieve the entity type
	const MetaType *entityType = metaData.FindType(entityTypeId);
	if(entityType == NULL)
		wxLogError(wxString::Format("Failed to find entity %04x ", entityTypeId));
	PBASSERT(entityType != NULL, "Unknown entity type");
	// for each object in the layer
	for(aqua::Array<SceneManager::Handle>::iterator h_it = handles.begin(); h_it != handles.end() ; ++h_it) {
		// fish out the name of the object for the entity
		const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
		aqua::shared_ptr<aqua::IGraphicObject> object = sceneManager->getObject(*h_it);
		aqua::Locator::checkIn(sceneManager);
		// car entity layer expects car body to be the parent of all the sub-objects
		if(object->nChildren() != 0) {
			std::string entityName(object->getObjectName());
			wxLogVerbose("Car is %s ", entityName.c_str());
			// create an entity instance
			DynamicMetaParser entityParser(metaData.AllocateData(entityName.c_str(), entityTypeId));
			DynamicMetaParser spatialParser(entityParser, CalcCRC("SpatialComponentData"));
			DynamicMetaParser bodyParser(entityParser, CalcCRC("GraphicModelComponentData"));
			DynamicMetaParser wheelParser(entityParser, CalcCRC("WheelEntityType"));
			aqua::Uint32 entityId = entityParser.GetId();
			DynamicMetaParser::Iterator entityIterator(entityParser);
			PBASSERT(entityIterator.IsValid(), "Could not parse entity");
			PBASSERT(entityIterator.GetTypeId() == CalcCRC("MetaEntityReference"), "self member is wrong type");
			entityIterator.Find("entityId").SetValue(entityId);			// set the "self"
			entityIterator.Next();										// skip the "parent" for now
			PBASSERT(entityIterator.GetTypeId() == CalcCRC("MetaEntityReference"), "parent member is wrong type");
			entityIterator.Next();
			// now do the components
			// split into wheels and damage layers
			std::vector< aqua::shared_ptr<const IGraphicObject> > wheels;
			std::vector< aqua::shared_ptr<const IGraphicObject> > damage_layers;
			for(int childIndex = 0; childIndex < object->nChildren(); ++childIndex) {
				aqua::HashString childHash = object->getChild(childIndex);
				const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
				aqua::shared_ptr<const aqua::IGraphicObject> childObject = sceneManager->getObject(sceneManager->getHandle(childHash));
				aqua::Locator::checkIn(sceneManager);
				std::string childName(childObject->getObjectName());
				// its a wheel
				if((childName.size() >= 5) && (childName.substr(0,5).compare("wheel") == 0)) {
					wheels.push_back(childObject);
				} else {
					// its a damage layer?
					damage_layers.push_back(childObject);
				}
			}
			for(int wheelIndex = 0; wheelIndex < 4; ++wheelIndex) {
				DynamicMetaParser wheelElementParser = wheelParser.Find(wheelIndex, MetaParser::FIND_BY_INDEX);
				PBASSERT(wheelElementParser.IsValid(), "Failed to find wheel");
				aqua::Uint32 wheelEntityId = AllocateWheel(wheelIndex, metaData, entityId, entityName, wheels[wheelIndex]);
				wheelElementParser.Find("entityId").SetValue(wheelEntityId);	// set the "self"
			}
			// do the cars spatial component
			wxString spatialComponentName(wxString::Format("Component_%04x_%04x", entityId, CalcCRC("SpatialComponentData")));
			DynamicMetaParser(spatialParser, CalcCRC("id")).SetValue(CalcCRC(spatialComponentName.c_str()));
			metaData.AllocateData(spatialComponentName.c_str(), CalcCRC("SpatialComponentData"));
			object->initialiseComponent(metaData, std::string(spatialComponentName.c_str()), CalcCRC("SpatialComponentData"));
			PBASSERT(bodyParser.IsValid(), "Unable to find graphic mesh components");
			DynamicMetaParser bodyElementParser = bodyParser.Find(0, MetaParser::FIND_BY_INDEX);
			wxString bodyComponentName(wxString::Format("Component_%04x_%04x_%02x", entityId, CalcCRC("GraphicModelComponentData", 0)));
			DynamicMetaParser(bodyElementParser, CalcCRC("id")).SetValue(CalcCRC(bodyComponentName.c_str()));
			metaData.AllocateData(bodyComponentName.c_str(), CalcCRC("GraphicModelComponentData"));
			object->initialiseComponent(metaData, std::string(bodyComponentName.c_str()), CalcCRC("GraphicModelComponentData"));
			// find the first damage layer
			aqua::shared_ptr< const IGraphicObject > damageLayerOne;
			for(std::vector< aqua::shared_ptr< const IGraphicObject > >::iterator dam_it = damage_layers.begin();
				dam_it != damage_layers.end(); ++dam_it) {
				std::string layerName((*dam_it)->getObjectName());
				if(layerName.substr(layerName.size()-2, 2).compare("01") == 0) {
					damageLayerOne = *dam_it;
				}
			}
			if(damageLayerOne == NULL)	 {
				wxLogError("Can't find damage Layer one");
				const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
				sceneManager->markAsUnexportable();
				aqua::Locator::checkIn(sceneManager);
				return false;
			}
			// fill in the array entry
			bodyElementParser = bodyParser.Find(1, MetaParser::FIND_BY_INDEX);
			bodyComponentName = wxString::Format("Component_%04x_%04x_%02x", entityId, CalcCRC("GraphicModelComponentData", 1));
			bodyElementParser.Find("id").SetValue(CalcCRC(bodyComponentName.c_str()));
			metaData.AllocateData(bodyComponentName.c_str(), CalcCRC("GraphicModelComponentData"));
			damageLayerOne->initialiseComponent(metaData, std::string(bodyComponentName.c_str()), CalcCRC("GraphicModelComponentData"));
			// find the second damage layer
			aqua::shared_ptr< const IGraphicObject > damageLayerTwo;
			for(std::vector< aqua::shared_ptr< const IGraphicObject > >::iterator dam_it = damage_layers.begin();
				dam_it != damage_layers.end(); ++dam_it) {
				std::string layerName((*dam_it)->getObjectName());
				if(layerName.substr(layerName.size()-2, 2).compare("02") == 0) {
					damageLayerTwo = *dam_it;
				}
			}
			if(damageLayerTwo == NULL) {
				wxLogError("Can't find damage layer two");
				const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
				sceneManager->markAsUnexportable();
				aqua::Locator::checkIn(sceneManager);
				return false;
			}
			PBASSERT(damageLayerTwo != NULL, "Can't find Damage Layer Two");
			// fill in the array entry
			bodyElementParser = bodyParser.Find(2, MetaParser::FIND_BY_INDEX);
			bodyComponentName = wxString::Format("Component_%04x_%04x_%02x", entityId, CalcCRC("GraphicModelComponentData", 2));
			bodyElementParser.Find("id").SetValue(CalcCRC(bodyComponentName.c_str()));
			metaData.AllocateData(bodyComponentName.c_str(), CalcCRC("GraphicModelComponentData"));
			damageLayerTwo->initialiseComponent(metaData, std::string(bodyComponentName.c_str()), CalcCRC("GraphicModelComponentData"));
			// third layer is identical to second (for now)
			bodyElementParser = bodyParser.Find(3, MetaParser::FIND_BY_INDEX);
			bodyComponentName = wxString::Format("Component_%04x_%04x_%02x", entityId, CalcCRC("GraphicModelComponentData", 3));
			bodyElementParser.Find("id").SetValue(CalcCRC(bodyComponentName.c_str()));
			metaData.AllocateData(bodyComponentName.c_str(), CalcCRC("GraphicModelComponentData"));
			damageLayerTwo->initialiseComponent(metaData, std::string(bodyComponentName.c_str()), CalcCRC("GraphicModelComponentData"));
		} // if nChildren is zero
	} // for each handle in layer
	return true;
}

int CarLayerExporter::LayerEntityCount(aqua::Array<SceneManager::Handle>& handles) {
	// simple: there's one for every max object!
	return handles.size();
}

bool CarLayerExporter::ExportDataLayer(const std::string& layerName, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles) {
	(void) handles;
	(void) metaData;
	(void) layerName;
	return false;
}
