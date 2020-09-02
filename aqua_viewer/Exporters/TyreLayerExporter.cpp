
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
#include "TyreLayerExporter.h"

using namespace aqua;

// TO DO : Many layers follow this pattern : hoik it out and template it
// TO DO : would be a good idea to validate types else where, prior to export and give better feedback
bool TyreLayerExporter::ExportLayer(aqua::Uint32 entityTypeId, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles)  {
	return SimpleLayerExporter::ExportLayer(entityTypeId, metaData, handles);
}
 
int TyreLayerExporter::LayerEntityCount(aqua::Array<SceneManager::Handle>& handles) {
	// simple: there's one for every max object!
	return SimpleLayerExporter::LayerEntityCount(handles);
}
