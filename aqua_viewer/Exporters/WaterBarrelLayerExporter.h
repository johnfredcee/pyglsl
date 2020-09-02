#ifndef WATERBARRELLAYEREXPORTER_H
#define WATERBARRELLAYEREXPORTER_H

class WaterBarrelLayerExporter : public SimpleLayerExporter
{
	virtual bool ExportLayer(aqua::Uint32 entityId, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles);
	virtual int  LayerEntityCount(aqua::Array<SceneManager::Handle>& handles);			
};

#endif
