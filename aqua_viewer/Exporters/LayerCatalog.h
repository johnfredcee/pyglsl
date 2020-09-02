#ifndef LAYERCATALOG_H
#define LAYERCATALOG_H

class LayerCatalog {
	public:
		static bool exportLayerCatalog(const std::string& layerName, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles);

};
#endif
