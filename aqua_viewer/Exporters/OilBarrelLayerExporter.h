#ifndef OILBARRELLAYEREXPORTER_H
#define OILBARRELLAYEREXPORTER_H

class OilBarrelLayerExporter : public SimpleLayerExporter
{
	virtual bool ExportLayer(aqua::Uint32 entityId, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles);
	virtual int  LayerEntityCount(aqua::Array<SceneManager::Handle>& handles);			
};

#endif
