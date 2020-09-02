#include "StdAfx.h"
#include <limits>

// all the meta types we need to render a scene
#include <aqua/VertexBufferData.h>
#include <aqua/VertexIndexBufferData.h>
#include <aqua/Material.h>
#include <aqua/TextureData.h>
#include <Utils/TextureLoader.h>

#include <wx/glcanvas.h>
#include <FileIO/aqua/MemoryObjectLoader.h>

#include "Viewer.h"
#include "ViewerFrame.h"

#include "MetaDataUtils.h"
#include "FbxParse.h"
#include "LayerManager.h"
#include "IGraphicObject.h"
#include "GraphicObject.h"
#include "LayerManager.h"
#include "MetaSerializer.h"
#include "SceneManager.h"
#include "MetaEditorPanel.h"
#include "MetaScenePanel.h"


using namespace aqua;

SceneManager::Handle SceneManager::next_handle_ = 0;

wxString SceneManager::headerDirectory_ = "Unknown";

/** Constructor */
SceneManager::SceneManager() : metaData_(new DynamicMetaData), validScene_(true), renderable_(false), parser_(NULL) {
	maxExtent_.x() = -std::numeric_limits<float>::max();
	maxExtent_.y() = -std::numeric_limits<float>::max();
	maxExtent_.z() = -std::numeric_limits<float>::max();
	minExtent_.x() = std::numeric_limits<float>::max();
	minExtent_.y() = std::numeric_limits<float>::max();
	minExtent_.z() = std::numeric_limits<float>::max();
	if (SceneManager::headerDirectory_ == "Unknown")
		findMetaHeaderDirectory();
	VarString includePath(SceneManager::headerDirectory_.utf8_str());
	metaData_->SetIncludePath(includePath);
	updateMetaDataHeaders();
	metaData_->ReadHeader(SceneManager::headerDirectory_ + "\\AquaViewerHeader.meta.h");
}

/**
 * Return the name of the scene
 * @return name
 */
std::string SceneManager::sceneName()
{
	return sceneName_;
}

/**
 * update the extents of the scene based on a newly added object
 * @param object <doc>
 */
void SceneManager::updateSceneExtents(aqua::shared_ptr<IGraphicObject> object) {
	const PbVector3& objectMax = object->getMaxExtent();
	if (maxExtent_.x() < objectMax.x()) {
		maxExtent_.x() = objectMax.x();
	}
	if (maxExtent_.y() < objectMax.y()) {
		maxExtent_.y() = objectMax.y();
	}
	if (maxExtent_.z() < objectMax.z()) {
		maxExtent_.z() = objectMax.z();
	}
	const PbVector3& objectMin = object->getMinExtent();
	if (minExtent_.x() > objectMin.x()) {
		minExtent_.x() = objectMin.x();
	}
	if (minExtent_.y() > objectMin.y()) {
		minExtent_.y() = objectMin.y();
	}
	if (minExtent_.z() > objectMin.z()) {
		minExtent_.z() = objectMin.z();
	}
}


/**
 * Get the extents of the scene (used to decide where to place the camera
 * @param minExtent
 * @param maxExtent
 */
void SceneManager::getSceneExtents(aqua::PbVector3& minExtent, aqua::PbVector3& maxExtent) const {
	minExtent = minExtent_;
	maxExtent = maxExtent_;
	return;
}

/* set the parser we use with this scene **/
void SceneManager::setParser(FbxParse* parser) {
	parser_ = parser;
}

/**
 * Retrieve pointer to parser used to parse scene
 * @return Pointer to parser
 */
FbxParse*  SceneManager::sceneParser() const {
	return parser_;
}

/**
 * Add a node to the scene
 * @param parser FbxParser for scene
 * @param pNode	 Node to add
 */
void SceneManager::addNodeToScene(FbxNode* pNode) {
	PBASSERT(parser_ != NULL, "Fbx Parser Not created")
		// first, work out what layer this node is in
		FbxDisplayLayer *layer = parser_->getDisplayLayer(pNode);
	// get the fdirbx / max layer name
	std::string layerName("static_furniture");
	if (layer != NULL)
		layerName = layer->GetName();
	// strip out any qualifications after
	std::string baseLayerName(LayerManager::getLayerName(layerName));
	shared_ptr< IGraphicObject > node(GraphicObject::create(parser_, pNode, metaData_));
	updateSceneExtents(node);
	next_handle_++;
	scene_[next_handle_] = node;
	handleMap_[node->getName()] = next_handle_;
	layers_[layerName].push_back(next_handle_);
	return;
}


/**
 * Allocate data in the scene manager
 * @param name Name of metadata instance
 * @param typeName Type of metadata instance to create
 * @return crc id of created instance
 */
DynamicMetaParser SceneManager::allocateData(const std::string& name, std::string& typeName) {
	wxLogDebug("Allocating Data Instance: %s  Type: %s ", name.c_str(), typeName.c_str());
	sceneIds_.push_back(CalcCRC(name.c_str()));
	return metaData_->AllocateData(name.c_str(), CalcCRC(typeName.c_str()));
}

/**
 * Get a named metadata instance from the scene
 * @param name name of instance to fetch
 * @return parser for taking apart instance, invalid if doesn't exist
 */
DynamicMetaParser SceneManager::fetchData(const std::string& name) {
	return DynamicMetaParser(*metaData_, name.c_str());
}

/**
 * Get a parser for a bit of metadata managed in the scene
 * @param id id of meta data instance
 * @return parser to pick apart the metadata
 */
DynamicMetaParser SceneManager::fetchData(crc32 id) {
	return DynamicMetaParser(*metaData_, id);
}

/**
 * Description for copyData.
 * @param dst <doc>
 * @param details <doc>
 */
void SceneManager::copyData(DynamicMetaData& dst, DynamicMetaData::DetailLevel details) {
	errCode status = dst.DeepCopy(*metaData_, details);
	PBASSERT(status == ERROR_CODE_OK, "Copy failed.");
	return;
}


/**
 * Test to see if the given instance name names an instance in the scene data
 * @param name Name of the instance to check for
 * @return true if it exists
 */
bool SceneManager::existsInScene(const std::string& name) {
	DynamicMetaParser testParse(*metaData_, CalcCRC(name.c_str()));
	return testParse.IsValid();
}

/**
 * Get a pointer to the object represented by the handle
 * @param handle Handle
 * @return Const shared pointer to object
 */
aqua::shared_ptr< const aqua::IGraphicObject >	SceneManager::getObject(SceneManager::Handle handle) const {
	SceneManager::sceneType::const_iterator it(scene_.find(handle));
	PBASSERT(it != scene_.end(), "Looking up non-existent object in scene");
	return it->second;
}

/**
 * Get a pointer to the object represented by the handle
 * @param handle Handle
 * @return shared pointer to object
 */
aqua::shared_ptr< aqua::IGraphicObject >  SceneManager::getObject(SceneManager::Handle handle) {
	SceneManager::sceneType::const_iterator it(scene_.find(handle));
	PBASSERT(it != scene_.end(), "Looking up non-existent object in scene");
	return it->second;
}

/**
 * Get a handle for the object given a hash string id
 * @param name id Of object
 * @return handle of object
 */
SceneManager::Handle SceneManager::getHandle(const aqua::HashString& name) const {
	handleMapType::const_iterator it(handleMap_.find(name));
	PBASSERT(it != handleMap_.end(), "Looking up non-existent object in scene");
	return it->second;
}

/**
 * Remove an object from the scene
 * @param handle Handle of object
 */
void SceneManager::removeFromScene(SceneManager::Handle handle) {
	sceneType::iterator it(scene_.find(handle));
	PBASSERT(it != scene_.end(), "Trying to remove non-existent object from scene");
	handleMapType::const_iterator handle_it(handleMap_.find(it->second->getName()));
	PBASSERT(handle_it != handleMap_.end(), "Inconsistent maps");
	if (it != scene_.end()) {
		scene_.erase(it);
		handleMap_.erase(handle_it);
	}
};

/**
 * Update the position of an object in the scene
 * @param handle handle of object
 * @param x x coord of new pos
 * @param y y coord of new pos
 * @param z z coord of new pos
 */
void SceneManager::setPosition(SceneManager::Handle handle, PbReal x, PbReal y, PbReal z) {
	sceneType::iterator it(scene_.find(handle));
	PBASSERT(it != scene_.end(), "Trying to modify non-existent object from sceene");
	aqua::shared_ptr< aqua::IGraphicObject > object = it->second;
	PbVector4 translation(object->getTransformMatrixPtr()->getColumn(3));
	translation.x() = x;
	translation.y() = y;
	translation.z() = z;
	object->getTransformMatrixPtr()->setColumn(3, translation);
}

class KeyExtractor
{
	public:
		KeyExtractor(aqua::Array<SceneManager::Handle>& handles) : handles_(handles) {
		};

		void operator()(const SceneManager::sceneType::value_type& item) {
			handles_.pushBack(item.first);
		}

		KeyExtractor& operator=(const KeyExtractor& other) {
			handles_ = other.handles_;
			return *this;
		}
	private:
		aqua::Array<SceneManager::Handle>& handles_;
};

class ElementExtractor
{
	public:
		ElementExtractor(aqua::Array<SceneManager::Handle>& handles) : handles_(handles) {
		};

		void operator()(const SceneManager::Handle& item) {
			handles_.pushBack(item);
		}

		ElementExtractor& operator=(const ElementExtractor& other) {
			handles_ = other.handles_;
			return *this;
		}
		
	private:
		aqua::Array<SceneManager::Handle>& handles_;
};

/**
 * Extracts a list of all layers making up the scene into a vector of strings
 * @param layerNames vector of strings that will contain names
 */
void SceneManager::getLayers(aqua::Array<std::string>& layerNames) const {
	for (sceneLayers::const_iterator it = layers_.begin(); it != layers_.end(); ++it)  {
		layerNames.pushBack(it->first);
	}
	return;
}

/**
 * Fill a vector with all the handles in the scene
 * @param handles vector of handles to fill
 */
void SceneManager::getSceneObjects(aqua::Array<SceneManager::Handle>& handles) const {
	KeyExtractor key_extractor(handles);
	std::for_each(scene_.begin(), scene_.end(), key_extractor);
}



/**
 * Fil a vector with the handles of all objects in the layer
 * @param layerName name of the layer
 * @param handles vector of handles to fill
 */
void SceneManager::getSceneObjectsInLayer(const std::string& layerName, aqua::Array<SceneManager::Handle>& handles) const {
	sceneLayers::const_iterator it = layers_.find(layerName);
	PBASSERT(it != layers_.end(), "Layer not found");
	ElementExtractor element_extractor(handles);
	std::for_each(it->second.begin(), it->second.end(), element_extractor);
}


/**
 * Compute the transform for the fbx node in the scene (possibly should be in fbxparse.cpp)
 * @param pNode Fbx node to compute transform of
 * @return shared_ptr to transform matrix
 */
aqua::shared_ptr<PbMatrix4> SceneManager::computeSceneTransform(FbxNode* pNode) {
	FbxAMatrix transform(getNodeTransform(pNode));
	aqua::shared_ptr<PbMatrix4> sceneTransform = fbxToPlayboxTransform(transform);
	return sceneTransform;
}

/**
 * Compute the transform for the fbx node in the scene relative to parent
 * @param pNode Fbx node to compute transform of
 * @return shared_ptr to transform matrix
 */
aqua::shared_ptr<PbMatrix4> SceneManager::computeRelativeSceneTransform(FbxNode* pNode) {
	FbxAMatrix transform(getNodeLocalTransform(pNode));
	aqua::shared_ptr<PbMatrix4> sceneTransform = fbxToPlayboxTransform(transform);
	return sceneTransform;
}



/**
 * Set the panel we are going to use to view this scene
 * @param view Meta Editor Panel that we use to view the scene
 */
void SceneManager::setView(MetaScenePanel* view) {
	view_=view;
}

/** Update the view with the current meta editor state */
void SceneManager::refreshView() {
	view_->refresh(metaData_);
}


/**
 * Test to see if this scene is valid for export
 * @return true if this scene is valid for export
 */
bool SceneManager::isExportable() const {
	return ((!scene_.empty()) && validScene_);
}

bool SceneManager::isRenderable() const {
	return renderable_;
}

/**
 * Mark this scene as unexportable
 */
void SceneManager::markAsUnexportable()	 {
	validScene_ = false;
}

void SceneManager::markAsRenderable() {
	renderable_ = true;
}

/** Destroy all objects in the scene and start afresh */
void SceneManager::clearScene() {
	layers_.clear();
	scene_.clear();
	handleMap_.clear();
	sceneIds_.clear();
	metaData_ = aqua::shared_ptr<DynamicMetaData>(new DynamicMetaData());
	// TODO: How can we avoid reading headers? Clone from some pre-prepared object?
	VarString headerDirectory(VarString(SceneManager::headerDirectory_.utf8_str()));
	metaData_->SetIncludePath(headerDirectory);
	updateMetaDataHeaders();
	metaData_->ReadHeader(SceneManager::headerDirectory_ + "\\AquaViewerHeader.meta.h");
	if (view_)
		view_->refresh(metaData_);
}




/**
 * Load data of a specific type from scene metadata to arrays
 * @param dataTypeName Name of type to extract from scene
 * @param ids Array to put ids in
 * @param objects Array to put objects in
 * @param memoryCategory memory category to use for allocation
 * @return true if successful
 */
template <typename T>
bool SceneManager::loadData(const char* dataTypeName,
							Array<const HashString>& ids,
							Array<T>& objects,
							const aqua::Int32  memoryCategory)
{

	errCode	  err;
	Uint32	  typeCrc(CalcCRC(dataTypeName));
	StreamBuf idStreamBuf(false);
	err = metaData_->WriteIds(idStreamBuf,typeCrc);
	PBASSERT(err == ERROR_CODE_OK, "Meta parsing failure");
	StreamBuf objectStreamBuf(false);
	err = metaData_->WriteObjects(objectStreamBuf,typeCrc);
	PBASSERT(err == ERROR_CODE_OK, "Meta parsing failure");
	StreamBuf elementStreamBuf(false);
	err = metaData_->WriteElements(elementStreamBuf, METADATA_TYPE_VARARRAY);
	PBASSERT(err == ERROR_CODE_OK, "Meta parsing failure");
	StreamBuf fixupStreamBuf(false);
	err = metaData_->WriteFixups(fixupStreamBuf, METADATA_TYPE_VARARRAY);
	PBASSERT(err == ERROR_CODE_OK, "Meta parsing failure");
	StreamBuf binBlockElementStreamBuf(false);
	err = metaData_->WriteElements(binBlockElementStreamBuf, METADATA_TYPE_BINBLOCK);
	PBASSERT(err == ERROR_CODE_OK, "Meta parsing failure");
	StreamBuf binBlockFixupStreamBuf(false);
	err = metaData_->WriteFixups(binBlockFixupStreamBuf, METADATA_TYPE_BINBLOCK);
	PBASSERT(err == ERROR_CODE_OK, "Meta parsing failure");
	u32 objectCount = idStreamBuf.Size() / sizeof(u32);
	PBASSERT(objectStreamBuf.Size() / sizeof(T) == objectCount, "Parsing problem");

	// Arrays hold the raw data in bytes (techincally should be binblocks)
	VarArray<u8> idData;
	idStreamBuf.Release(idData);
	VarArray<u8> objectData;
	objectStreamBuf.Release(objectData);
	VarArray<u8> elementData;
	elementStreamBuf.Release(elementData);
	VarArray<u8> fixupData;
	fixupStreamBuf.Release(fixupData);
	VarArray<u8> binBlockElementData;
	binBlockElementStreamBuf.Release(binBlockElementData);
	VarArray<u8> binBlockFixupData;
	binBlockFixupStreamBuf.Release(binBlockFixupData);
	aqua::Array< VarArray<u8>> data;
	data.pushBack(idData);
	data.pushBack(objectData);
	data.pushBack(elementData);
	data.pushBack(fixupData);
	data.pushBack(binBlockElementData);
	data.pushBack(binBlockFixupData);
	return MemoryObjectLoader::loadObjects<T>(&ids, &objects, objectCount, data, memoryCategory);
}

/** loadScene() - load the scene from metatata into buffer managers */
void SceneManager::loadScene() {

	HashString toolGroupName("ToolData");

	Array<const HashString>	 vbufferids;
	Array<VertexBufferData>	 vbuffers;

	Array<const HashString>		  vibufferids;
	Array<VertexIndexBufferData>  vibuffers;

	Array<const HashString>	  matbufferids;
	Array<Material>			  matbuffers;

	Array<const HashString>	  bitmapbufferids;
	Array<BitmapData>		  bitmapbuffers;

	Array<const HashString>	  texturebufferids;
	Array<TextureData>		  texturebuffers;

	loadData<VertexBufferData>("VertexBufferData", vbufferids, vbuffers);
	loadData<VertexIndexBufferData>("VertexIndexBufferData", vibufferids, vibuffers);
	loadData<Material>("Material", matbufferids, matbuffers);
	loadData<BitmapData>("BitmapData", bitmapbufferids, bitmapbuffers);
	for(Uint32 i = 0; i < bitmapbufferids.size(); i++) {
		wxLogDebug(wxString::Format("BitmapId %08x", bitmapbufferids[i].getHashValue()));
	}
	loadData<TextureData>("TextureData", texturebufferids, texturebuffers);
	for(Uint32 i = 0; i < bitmapbufferids.size(); i++) {
		wxLogDebug(wxString::Format("TextureId %08x", texturebufferids[i].getHashValue()));
	}

	vertexBufferManager_.createStaticGroup(toolGroupName);
	vertexIndexBufferManager_.createStaticGroup(toolGroupName);

	vertexBufferManager_.insertStaticGroup(toolGroupName, vbufferids, vbuffers);
	vertexIndexBufferManager_.insertStaticGroup(toolGroupName, vibufferids, vibuffers);

	const Locator::Service<MaterialManager>& materialManager(Locator::checkOut<MaterialManager>());
	materialManager->createStaticGroup(toolGroupName);
	materialManager->insertStaticGroup(toolGroupName, matbufferids, matbuffers);
	Locator::checkIn(materialManager);

	TextureLoader::loadTextures(toolGroupName,
								bitmapbufferids, bitmapbuffers,
								texturebufferids, texturebuffers);

	renderable_ = true;
}

/** Description for unloadScene. */
void SceneManager::unloadScene() {
	if (renderable_) {
		renderable_ = false;

		HashString toolGroupName("ToolData");
		vertexBufferManager_.removeStaticGroup(toolGroupName);
		vertexIndexBufferManager_.removeStaticGroup(toolGroupName);
		const Locator::Service<MaterialManager>& materialManager(Locator::checkOut<MaterialManager>());
		materialManager->removeStaticGroup(toolGroupName);
		Locator::checkIn(materialManager);
		const Locator::Service<TextureManager>& textureManager(Locator::checkOut<TextureManager>());
		textureManager->removeStaticGroup(toolGroupName);
		aqua::Locator::checkIn(textureManager);
	}
}

/** Draw the scene prior to metadataisation? */
void SceneManager::renderScene() {

	aqua::Array<Handle> handles;
	getSceneObjects(handles);
	aqua::Array<Handle>::iterator h_it;
	for(h_it = handles.begin(); h_it != handles.end(); ++h_it) {
		aqua::shared_ptr< const aqua::IGraphicObject > object = getObject( *h_it );
		object->render();
	}
	return;
}

