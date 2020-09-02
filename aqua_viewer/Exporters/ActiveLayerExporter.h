#ifndef ACTIVELAYEREXPORTER_H
#define ACTIVELAYEREXPORTER_H

class ActiveLayerExporter : public SimpleLayerExporter
{
	virtual bool ExportLayer(aqua::Uint32 entityId, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles);
	virtual int  LayerEntityCount(aqua::Array<SceneManager::Handle>& handles);			
};

#endif
