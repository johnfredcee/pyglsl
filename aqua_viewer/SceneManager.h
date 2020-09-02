#if !defined(SCENEMANAGER_H_INCLUDED)
#define SCENEMANAGER_H_INCLUDED

#include <list>
#include <vector>
#include <Locator.h>
#include <Types.h>
#include <Managers/VertexBufferManager.h>
#include <Managers/VertexIndexBufferManager.h>
#include "IGraphicObject.h"
#include "HashComparator.h"

class DynamicMetaParser;
class MetaScenePanel;
class FbxParse;
class LayerManager;

namespace aqua {
	class IGraphicObject;
}

namespace FBXSDK_NAMESPACE {
	class FbxNode;
}


template <typename T> void MetaSerialiser(const T& data, DynamicMetaParser& parser);

template <typename T> void MetaDeserialiser(T& data, DynamicMetaParser& parser);

/** < We assemble game objects in this "Scene" container prior to export. Every game object has a handle associated with it and is addressed via th handle */
class SceneManager : public aqua::Locator::ServiceSingleton<SceneManager> {

		template <typename T> friend void MetaSerialiser(const T& data, DynamicMetaParser& parser);

		template <typename T> friend void MetaDeserialiser(T& data, DynamicMetaParser& parser);

		friend class aqua::Locator::ServiceSingleton<SceneManager>;

		friend void findMetaHeaderDirectory();

		friend void updateMetaDataHeaders();

		friend void readMetaDataHeaders(DynamicMetaData& md, const wxString subDirectory);

		friend class LayerManager;

	public:
		typedef int Handle;

		typedef std::map<aqua::HashString, Handle, HashComparator>		   handleMapType;
		typedef std::map<Handle, aqua::shared_ptr< aqua::IGraphicObject> > sceneType;
		typedef std::list<Handle>										   layerHandles;
		typedef std::map<std::string,  layerHandles >					   sceneLayers;
		typedef std::vector<crc32>										   sceneIds;

		/** < C'tor */
		SceneManager::SceneManager();


		/* set the parser we use with this scene **/
		void setParser(FbxParse* parser);

		/**
		 * Return the current scene name
		 * @return sceneName
		 */
		std::string sceneName();
		
		/**
		 * Retrieve pointer to parser used to parse scene
		 * @return Pointer to parser
		 */
		FbxParse*    sceneParser() const;

		/**
		 * Set the panel we are going to use to view this scene
		 * @param view Meta Editor Panel that we use to view the scene
		 */
		void setView(MetaScenePanel* view);


		/**
		 * Refresh the meta editor view
		 */
		void refreshView();

		/**
		 * Add a node to the scene
		 * @param parser FbxParser for scene
		 * @param pNode	 Node to add
		 */
		void addNodeToScene(FBXSDK_NAMESPACE::FbxNode* pNode);


		/**
		 * Allocate data in the scene manager
		 * @param name Name of metadata instance
		 * @param typeName Type of metadata instance to create
		 * @return crc id of created instance
		 */
		DynamicMetaParser allocateData(const std::string& name, std::string& typeName);

		/**
		 * Fetch scene object as metadata
		 * @param name name of metadata
		 * @return parser to parse metadata
		 */
		DynamicMetaParser fetchData(const std::string& name);

		/**
		 * Fetch data from scene by id
		 * @param id id of data to fetch (crc32 name)
		 * @return parser to parse metadata
		 */
		DynamicMetaParser fetchData(crc32 id);

		/**
		 * Description for copyData.
		 * @param details How much detail to copy
		 */
		void copyData(DynamicMetaData& dst, DynamicMetaData::DetailLevel details = DynamicMetaData::DETAIL_FULL);

		/**
		 * Test to see if the given instance name names an instance in the scene data
		 * @param name Name of the instance to check for
		 * @return true if it exists
		 */
		bool existsInScene(const std::string& name);

		/**
		 * Extract a pointer to the object
		 * @param handle handle of object to get
		 * @return object pointer_type
		 */
		aqua::shared_ptr< aqua::IGraphicObject > getObject(int handle);

		/**
		 * Extract a const pointer to the object
		 * @param handle handle of object to get
		 * @return object pointer_type (const)
		 */
		aqua::shared_ptr< const aqua::IGraphicObject > getObject(Handle handle) const;

		/**
		 * Fish out the handle of an object, given it's name. Asserts if
		 * not found.
		 * @param name name of object
		 * @return value of handle.
		 */
		Handle getHandle(const aqua::HashString& name) const;


		/**
		 * Remove instance from the scene
		 * @param handle handle to instance to remove
		 */
		void removeFromScene(Handle handle);


		/**
		 * Update the posiiton of the instance in the scene and
		 * recalculate the bounding box
		 * @param handle <doc>
		 * @param x <doc>
		 * @param y <doc>
		 * @param z <doc>
		 */
		virtual void setPosition(Handle handle, aqua::PbReal x, aqua::PbReal y, aqua::PbReal z);

		/**
		 * Description for getSceneObjects.
		 * @param handles Array to fill with handles
		 */
		void getSceneObjects(aqua::Array<Handle>& handles) const;

		/**
		 * Description for getSceneObjects.
		 * @param layerNames array to fill with layer names
		 */
		void getLayers(aqua::Array<std::string>& layerNames) const;

		/**
		 * Description for getSceneObjectsInLayer
		 * @param layerName Name of layer to extract handles from
		 * @param handles Array to fill with handles
		 */
		void getSceneObjectsInLayer(const std::string& layerName, aqua::Array<Handle>& handles) const;

		/**
		 * Extract the scene transform for a node and squelch it into
		 * playbox format
		 * @param pNode Fbx Scene Node to calculate transform for
		 * @return aqua::shared_ptr<PbMatrix4> pointing to the scene
		 *		  transform
		 */
		aqua::shared_ptr<aqua::PbMatrix4> computeSceneTransform(FBXSDK_NAMESPACE::FbxNode* pNode);

		/**
		 * Extract the scene transform for a node and squelch it into
		 * playbox format
		 * @param pNode Fbx Scene Node to calculate transform for
		 * @return aqua::shared_ptr<PbMatrix4> pointing to the scene
		 *		  transform
		 */
		aqua::shared_ptr<aqua::PbMatrix4> SceneManager::computeRelativeSceneTransform(FBXSDK_NAMESPACE::FbxNode* pNode);

		/**
		 * Test to see if this scene is valid for export
		 * @return true if this scene is valid for export
		 */
		bool isExportable() const;

		/** Check to see if the scene can be drawn **/
		bool isRenderable() const;

		/**
		 * Mark this scene as unexportable
		 */
		void markAsUnexportable();

		/** Flag the scene as drawable */
		void markAsRenderable();

		/** Destroy everything in the scene */
		void clearScene();

		/** remove metadata */
		void unloadScene();

		/** load the metadata **/
		void loadScene();

		/** render scene metadata **/
		void renderScene();

		/** return the bounding box encompassing the scene */
		void getSceneExtents(aqua::PbVector3& minExtent, aqua::PbVector3& maxExtent) const;


	protected:

		/** < name of the scene being managed */
		std::string sceneName_;

		/** < metadata that describes the scene */
		aqua::shared_ptr<DynamicMetaData> metaData_;

		/** < Holds the actual scene */
		sceneType scene_;

		/** < maps names to handles */
		handleMapType handleMap_;

		/** < keeps a list of handles on each layer */
		sceneLayers	  layers_;

		/** holds each top-level scene id */
		sceneIds      sceneIds_;

		/** Meta Editor Panel used to view the scene */
		MetaScenePanel* view_;

		/** Flag whether this scene is valid for export */
		bool validScene_;

		/** Flag whether this scene can be rendered */
		bool renderable_;

		/** we use these to store processed metadata */
		aqua::VertexBufferManager vertexBufferManager_;

		/** we use these to store processed metadata */
		aqua::VertexIndexBufferManager vertexIndexBufferManager_;

		/* this is the parser we are using */
		FbxParse* parser_;

		//!< minimum extent of bounding box
		aqua::PbVector3 minExtent_;

		//!< maximum extent of bounding box
		aqua::PbVector3 maxExtent_;

	private:
		/** extend the bounding box around the scene if needed */
		void updateSceneExtents(aqua::shared_ptr<aqua::IGraphicObject> object);

		template <typename T>
		bool loadData(const char* dataTypeName,
					  aqua::Array<const aqua::HashString>& ids,
					  aqua::Array<T>& objects,
					  const aqua::Int32  memoryCategory = MEM_DEFAULT);


		static wxString	 headerDirectory_;

		static int next_handle_;

};

#endif



