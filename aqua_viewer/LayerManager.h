#ifndef LAYERMANAGER_H
#define LAYERMANAGER_H

#include "Locator.h"

class SceneManager;

class DynamicMetaData;
class DynamicMetaParser;
class ILayerExporter;

/** LayerManager.  The Layer Manager exists to map layers (Layers in
	max, DisplayLayers in fbx) to specific entities and export
	methods. The Export methods are defined in the classes in Exporter
	subdirectories. And convert metadata to entities, extracting any
	needed components as they go along. */
class LayerManager : public aqua::Locator::ServiceSingleton<LayerManager>
{
   friend class aqua::Locator::ServiceSingleton<LayerManager>;
	
public:

	class Layer
	{
	public:
		// constructor
		Layer(const std::string& entityName, aqua::shared_ptr<ILayerExporter> exporter, const std::string& regexp) :
			entity_(entityName), exporter_(exporter)  {
			regex_.Compile(wxString(regexp.c_str()), wxRE_DEFAULT);
			PBASSERT(regex_.IsValid(), "Malformed regex");
		}

		// the entity we expect to find in this layer
		std::string entity_;

		// methods used to export this layer
		aqua::shared_ptr<ILayerExporter> exporter_;

		// regular expression used to check names of things in this layer
		wxRegEx         regex_;
	};

	// map that matches layers to layer export methods
	typedef std::map<std::string, aqua::shared_ptr<Layer> > LayerDictionary;

public:

	//! Adds a layer to the dictionary with the given name, entity type to parse, and a shared_ptr to the exporter to call.
	bool addLayer(const std::string& name, const std::string& entityName, aqua::shared_ptr<ILayerExporter> exporter, const std::string& regexp);

	//! Returns the base layer name computed from an FBX-parsed layer name.
	static std::string getLayerName(const std::string& name);

	//! Checks to see if a layer object is valid for a given layer
	bool validForLayer(const std::string& layerName, const std::string& name);

	//! Exports a layer based upon it's name into the given Meta Data object.
	bool exportLayer(const std::string& name);

	//! Returns the layer name based upon the entity type given.
	const std::string getLayerNameFromEntityType(const std::string& entityType) const;

	//! Given the name of an export method, returns the appropiate functor
	aqua::shared_ptr<ILayerExporter> newExporterObject(const wxString& method);

	//! Layer catalog is a list of ids of each object exported for that layer
	static void getLayerCatalog(const std::string& layerName, DynamicMetaData& metaData, DynamicMetaParser& catalogParser);

	std::string getCurrentLayerName();

private:
	LayerDictionary layers_;
	std::string     currentLayerName_;

};

extern LayerManager gLayerManager;

#endif
