#include <StdAfx.h>

#include <PbSystemTypes.h>
#include <Locator.h>
#include <Utils/HashString.h>
#include <Utils/Logger.h>
#include <MetaData/Dynamic/DynamicMetaData.h>

#include <wx/glcanvas.h>

#include "../FbxParse.h"
#include "../SceneManager.h"

#include "ILayerExporter.h"
#include "SimpleLayerExporter.h"
#include "FurnitureLayerExporter.h"

using namespace aqua;

bool FurnitureLayerExporter::ExportLayer(aqua::Uint32 entityTypeId, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles)  {
	return SimpleLayerExporter::ExportLayer(entityTypeId, metaData, handles);
}
 
int FurnitureLayerExporter::LayerEntityCount(aqua::Array<SceneManager::Handle>& handles) {
	return SimpleLayerExporter::LayerEntityCount(handles);
}
