
#include <StdAfx.h>

#include <PbSystemTypes.h>
#include <Locator.h>
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
#include "DataLayerExporter.h"
#include "../LayerManager.h"
#include "LayerCatalog.h"

bool DataLayerExporter::ExportLayer(aqua::Uint32 dataTypeId, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles)	 {

	(void) dataTypeId;
	(void) metaData;
	PBUNREFERENCED_PARAMETER(handles);
	return false;
}

bool DataLayerExporter::ExportDataLayer(const std::string& layerName, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles)  {
	return LayerCatalog::exportLayerCatalog(layerName, metaData, handles);
}


int DataLayerExporter::LayerEntityCount(aqua::Array<SceneManager::Handle>& handles) {
	(void) handles;
	return 1; // just the layer catalog
}
