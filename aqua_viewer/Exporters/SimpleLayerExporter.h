#ifndef SIMPLEEXPORTERLAYER_H
#define SIMPLEEXPORTERLAYER_H



class SimpleLayerExporter : public ILayerExporter
{
protected:
	virtual bool ExportLayer(aqua::Uint32 entityId, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles);
	virtual bool ExportDataLayer(const std::string& layerName, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles);
	virtual int  LayerEntityCount(aqua::Array<SceneManager::Handle>& handles);		
};

#endif
