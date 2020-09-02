#ifndef SKINNEDLAYEREXPORTER_H
#define SKINNEDLAYEREXPORTER_H

class SkinnedLayerExporter : public SimpleLayerExporter
{   
	virtual bool ExportLayer(aqua::Uint32 entityId, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles);
	virtual bool ExportDataLayer(const std::string& layerName, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles);		
	virtual int  LayerEntityCount(aqua::Array<SceneManager::Handle>& handles);			
};

#endif
