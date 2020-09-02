
#include <StdAfx.h>

#include <PbSystemTypes.h>
#include <Locator.h>
#include <Utils/HashString.h>
#include <Utils/Logger.h>
#include <CRC/CRC.h>
#include <MetaData/Dynamic/DynamicMetaData.h>
#include <MetaData/Dynamic/DynamicMetaParser.h>
#include <wx/glcanvas.h>
#include <aqua/AnimationData.h>


#include "../FbxParse.h"
#include "../SceneManager.h"

#include "ILayerExporter.h"
#include "SimpleLayerExporter.h"
#include "../AnimationTimeStamp.h"
#include "../AnimationTranslationKey.h"
#include "../AnimationRotationKey.h"
#include "../AnimationKey.h"
#include "../AnimationTrack.h"
#include "../AnimationContainer.h"
#include "../MetaSerializer.h"
#include "../Animation.h"
#include "../Rtti.h"
#include "../Config.h"
#include "../IntermediateMesh.h"
#include "../IMesh.h"
#include "../IGraphicObject.h"
#include "../GraphicObject.h"
#include "../MeshObject.h"
#include "../LayerManager.h"
#include "ILayerExporter.h"
#include "SimpleLayerExporter.h"
#include "SkinnedLayerExporter.h"


using namespace aqua;


/**
 * This is for exporting in-scene animated objects. As well as exporting furniture entities, it looks for associated
 * animation data as well
 * @param dataTypeId Entity type assocated with this layer
 * @param metaData Metadata object we add metadata to (obsolete)
 * @param handles Handles to objects managed by scene manager
 * @return success in the case of successful export
 */
bool SkinnedLayerExporter::ExportLayer(aqua::Uint32 dataTypeId, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles)	 {

	
	(void) metaData;
	PBUNREFERENCED_PARAMETER(handles);

	// we need to look for any entities in here
	const aqua::Locator::Service<LayerManager>& layerManager(aqua::Locator::checkOut<LayerManager>());
	const std::string layerName(layerManager->getCurrentLayerName());
	// neeed these to access stuff
	const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());

	// TO DO -- look for skeleton and animation inside mesh objects and export
	
	aqua::Locator::checkIn(sceneManager);
	aqua::Locator::checkIn(layerManager);
	
	// export the actual entities
	return SimpleLayerExporter::ExportLayer(dataTypeId, metaData, handles);
}

bool SkinnedLayerExporter::ExportDataLayer(const std::string& layerName, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles)  {
	(void) handles;
	(void) metaData;
	(void) layerName;
	return false;	
}


int SkinnedLayerExporter::LayerEntityCount(aqua::Array<SceneManager::Handle>& handles) {
	(void) handles;
	return SimpleLayerExporter::LayerEntityCount(handles);
}
 
