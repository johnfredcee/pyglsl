#ifndef _STATIC_PHYSICS_LAYER_EXPORTER_H_
#define _STATIC_PHYSICS_LAYER_EXPORTER_H_

class StaticPhysicsLayerExporter : public ILayerExporter
{
	virtual bool ExportLayer(aqua::Uint32 typeId, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles);
	virtual bool ExportDataLayer(const std::string& layerName, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles);
	virtual int  LayerEntityCount(aqua::Array<SceneManager::Handle>& handles);			
};

#endif
