#ifndef ILAYEREXPORTER_H
#define ILAYEREXPORTER_H

class ILayerExporter {
public:
	virtual bool ExportLayer(aqua::Uint32 entityId, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles) = 0;
	virtual bool ExportDataLayer(const std::string& layerName, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles) = 0;
	virtual int  LayerEntityCount(aqua::Array<SceneManager::Handle>& handles) = 0;
};

#endif
