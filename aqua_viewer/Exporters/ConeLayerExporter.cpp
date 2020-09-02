
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
#include "ConeLayerExporter.h"
#include "../LayerManager.h"

using namespace aqua;


bool ConeLayerExporter::ExportLayer(aqua::Uint32 entityTypeId, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles)  {
	return SimpleLayerExporter::ExportLayer(entityTypeId, metaData, handles);
}
 
int ConeLayerExporter::LayerEntityCount(aqua::Array<SceneManager::Handle>& handles) {
	// simple: there's one for every max object!
	return SimpleLayerExporter::LayerEntityCount(handles);
}
