#include <StdAfx.h>

#include <PbSystemTypes.h>
#include <Locator.h>
#include <Utils/HashString.h>
#include <Utils/Logger.h>
#include <MetaData/Dynamic/DynamicMetaData.h>

#include <wx/glcanvas.h>


#include "../FbxParse.h"
#include "../SceneManager.h"
//#include "../MaterialNameManager.h"
#include "../GraphicObject.h"

#include "ILayerExporter.h"
#include "../LayerManager.h"
#include "StaticPhysicsLayerExporter.h"

using namespace aqua;

bool StaticPhysicsLayerExporter::ExportLayer(Uint32 typeId, DynamicMetaData& metaData, Array<SceneManager::Handle>& handles)
{
	(void) typeId;
	(void) metaData;
	(void) handles;
/* BROKEN BY THE REMOVAL OF MATERIALNAMEMANAGE .. NEED TO FIX
   
	const MetaType* typeCheck(metaData.FindType(typeId));
	if (typeCheck == NULL)
		wxLogError(wxString::Format("Failed to find type %04x ", typeId));
	PBASSERT(typeCheck != NULL, "Unknown component type");

	MaterialNameManager* materialNameManager(MaterialNameManager::instance());

	// Find the layer name and meta type name this exporter is dealing with.
	const char* exportType(metaData.FindString(typeId));
	if (!exportType) {
		PBBREAK("Cannot find Layer Name for object of type: " << typeId);
		wxLogError(wxString::Format("Cannot find Layer Name for object of type: %04x", typeId));
		gAllowExport = false;
		return false;
	}

	const aqua::Locator::Service<LayerManager>& layerManager(aqua::Locator::checkOut<LayerManager>());		
	const std::string layerName(layerManager->getCurrentLayerName());
	aqua::Locator::checkIn(layerManager);

	
	std::vector<aqua::Uint32> physicsCatalogIds;
	for (Array<SceneManager::Handle>::const_iterator handle(handles.begin()); handle != handles.end() ; ++handle) {
		const aqua::Uint TO_DO_NEED_TO_HAVE_GAME_OBJECT_MMATERIAL_ACCESS(0U);
#if 0		
		// neeed these to access stuff
		const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());										
		const shared_ptr<const IGraphicObject> iObject(sceneManager->getObject(*handle));
		aqua::Locator::checkIn(sceneManager);
		const shared_ptr<const GraphicObject> object(boost::dynamic_pointer_cast<const GraphicObject>(iObject));
		if (object) {
			const PbString& objectName(iObject->getObjectName());
			const Uint materialCount(object->getBufferCount());
			for (Uint materialIndex(0U); materialIndex < materialCount; ++materialIndex) {
				const MaterialId materialId(object->getMaterialId(materialIndex));
				const PbString& materialName(materialNameManager->getName(materialId));

				// Create each Physics Mesh Instance
				DynamicMetaParser physicsParser(metaData.AllocateData(PbString(objectName + "_" + materialName).c_str(), typeId));
				physicsCatalogIds.push_back(physicsParser.GetId());
				DynamicMetaParser::Iterator physicsIterator(physicsParser);

				PBASSERT(physicsIterator.IsValid(), "Could not create Physics Mesh");
				PBASSERT(physicsIterator.GetTypeId() != typeId, "Physics Iterator is not of type CollisionMeshData");

				physicsIterator.SetValue(PbString(objectName + "_" + materialName + "_vertices").c_str());	// Vertex Buffer Name
				physicsIterator.Next();
				physicsIterator.SetValue(PbString(objectName + "_" + materialName + "_indices").c_str());	// Index Buffer Name
				physicsIterator.Next();
				physicsIterator.SetValue(PbString(materialName).c_str());									// Material Name
				physicsIterator.Next();
				if (wxString(materialName.c_str()).AfterLast('_').Capitalize() == "Wall")					// Collision Group Name
					physicsIterator.SetValue("CollisionGroup_Wall");
				else
					physicsIterator.SetValue("CollisionGroup_Floor");
			}			
		}
#endif
	}
	
	// now for the catalog
	// create the catalog
	DynamicMetaParser catalogParser;
	LayerManager::getLayerCatalog(layerName.c_str(),  metaData,  catalogParser ); 
	PBASSERT(catalogParser.IsValid(), "Error allocating catalog parser");
	DynamicMetaParser idsParser(catalogParser, CalcCRC("layerIds")); 
	PBASSERT(idsParser.IsValid(), "Error allocating layer catalog for layer ");
	int startIndex = idsParser.GetArrayCount();
	idsParser.SetArrayCount(physicsCatalogIds.size() + startIndex);	
	for(std::vector<aqua::Uint32>::iterator p_it = physicsCatalogIds.begin(); p_it != physicsCatalogIds.end(); ++p_it) {
		idsParser.Find(startIndex + (p_it - physicsCatalogIds.begin()), MetaParser::FIND_BY_INDEX).SetValue(*p_it);	
	}
*/	
	return true;
}

bool StaticPhysicsLayerExporter::ExportDataLayer(const std::string& layerName, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles) {
	(void) layerName;
	(void) metaData;
	(void) handles;
	return false;
}

int StaticPhysicsLayerExporter::LayerEntityCount(Array<SceneManager::Handle>& handles)
{
	(void) handles;
	// simple: there's one for every max object!
	return handles.size();
}




