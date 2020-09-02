#ifndef CARLAYEREXPORTER_H
#define CARLAYEREXPORTER_H

class CarLayerExporter : public ILayerExporter
{
private:
	aqua::Uint32 AllocateWheel(int wheelIndex, DynamicMetaData& metaData, aqua::Uint32 entityId, std::string& entityName, aqua::shared_ptr<const aqua::IGraphicObject>& wheel);

public:	
	virtual bool ExportLayer(aqua::Uint32 entityId, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles);
	virtual bool ExportDataLayer(const std::string& layerName, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles);	
	virtual int  LayerEntityCount(aqua::Array<SceneManager::Handle>& handles);			
};

#endif
