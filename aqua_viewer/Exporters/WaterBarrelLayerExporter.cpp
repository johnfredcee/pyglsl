
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
#include "WaterBarrelLayerExporter.h"

using namespace aqua;

bool WaterBarrelLayerExporter::ExportLayer(aqua::Uint32 entityTypeId, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles)
{
	return SimpleLayerExporter::ExportLayer(entityTypeId, metaData, handles);
}

int WaterBarrelLayerExporter::LayerEntityCount(aqua::Array<SceneManager::Handle>& handles)
{
	return SimpleLayerExporter::LayerEntityCount(handles);
}
