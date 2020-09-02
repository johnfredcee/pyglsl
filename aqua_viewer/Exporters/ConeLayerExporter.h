#ifndef CONELAYEREXPORTER_H
#define CONELAYEREXPORTER_H

class ConeLayerExporter : public SimpleLayerExporter
{
	virtual bool ExportLayer(aqua::Uint32 entityId, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles);
	virtual int  LayerEntityCount(aqua::Array<SceneManager::Handle>& handles);			
};

#endif
