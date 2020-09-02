
#include <Utils/HashString.h>
#include <Graphs/Node.h>
#include "IGraphicObject.h"

class FbxParse;

namespace FBXSDK_NAMESPACE {
	class FbxNode;
};

namespace aqua {

	class GraphicObject : public IGraphicObject
	{
		public:
			/**
			 * Description for create.
			 * @param parser scene parser that contains node
			 * @param node Fbx node to create graphic object from
			 * @return pointer to newly created object
			 */
			static shared_ptr< IGraphicObject > create(FbxParse *parser, FBXSDK_NAMESPACE::FbxNode* node, shared_ptr<DynamicMetaData> metaData);


			/**
			 * Initialise: Set common fields
			 * @param pNode node we are initalising object from
			 */
			void initialise(FbxNode *pNode);


			//! return pointer to the scene transform of this object
			virtual shared_ptr< const PbMatrix4 > getTransformMatrixPtr() const;

			//! return pointer to the scene transform of this object (relative to parent)
			virtual shared_ptr< const PbMatrix4 > getRelativeTransformMatrixPtr() const;

			//! return pointer to the scene transform of this objects parent
			virtual shared_ptr< const PbMatrix4 > getParentTransformMatrixPtr() const;
			
			//! return the scene transform of this object
			virtual PbMatrix4 transform();

			//! set the position of this object in the scene
			virtual void setPosition(PbReal x, PbReal y, PbReal z);

			//! return the position of this object in the scene
			virtual void getPosition(PbReal& x, PbReal& y, PbReal& z) const;

			//! return the name of the object
			virtual const std::string& getObjectName() const ;

			//! set the name of the object
			virtual void setObjectName( const std::string& objectName );

			//! get the hashed id of the object
			virtual const HashString& getName() const;

			//! get the hint associated with the object
			virtual const std::string& getHint() const;

			//! get the hashed id of the parent
			virtual HashString getParent() const;

			//! get the name of the parent
			virtual const std::string& getParentName(void) const;
			
			//! add a child node
			virtual void  addChild(HashString child);

			//! get name of an indexed child
			virtual const HashString& getChild(int n) const;

			//! number of children
			virtual int nChildren() const;

			/**
			 * Return the original fbx node
			 * @return pointer to fbx node
			 */
			FBXSDK_NAMESPACE::FbxNode* getFbxNode();

			/**
			 * Transform to metadata
			 * @param  <doc>
			 */
			virtual void serialiseSelf(DynamicMetaData &) const;

			/**
			 * Description for deserialiseSelf.
			 * @param  recreate from metadata
			 */
			virtual void deserialiseSelf(DynamicMetaData &);


			/**
			 * Return "Type" of graphic object - ie, mesh, helper, spline, etc
			 * @param  <doc>
			 * @return <doc>
			 */
			virtual Rtti type(void) const;

			/**
			 * Description for ids.
			 * @param ids return ids of metatada associated with this object
			 * @return vector of CRCS to identify metadata instances
			 */
			virtual bool ids(std::vector<crc32>& ids);

			/**
			 * Initialise component related to this thing
			 *
			 * @param  MetaData to insert componenet in
			 * @param  Name of component to use as meta data instance id
			 * @param  Id of type to use for meta data instance
			 * @return true if successful
			 */
			virtual bool initialiseComponent(DynamicMetaData &metaData,const std::string &componentName, aqua::Uint32 componentTypeId) const;


			//! return the minimum extent of the bounding box
			virtual const PbVector3 getMinExtent() const;

			//! return the maximum extent of the bounding box
			virtual const PbVector3 getMaxExtent() const;

			/** Actually draw the thing */
			virtual void render() const;

		protected:
			virtual shared_ptr< PbMatrix4 > getTransformMatrixPtr();

			//!< object name as hash string 
			HashString	 name_;

			//!< Object name
			std::string objectName_;

			//!< Hint (aka comment)
			std::string hint_;

			//!< Transform matrix to orient and position the game in game
			shared_ptr< PbMatrix4 > transformMatrix_;

			//!< Transform matrix to orient and position the game in game
			shared_ptr< PbMatrix4 > relativeTransformMatrix_;

			//!< Transform matrix of parent
			shared_ptr< PbMatrix4 > parentTransformMatrix_;
			
			//!< Parent object (0 if parent is root)
			HashString parent_;

			//!< Name of parent object
			std::string parentName_;
			
			//!< Scene chidren of this object
			Array<HashString> children_;

			//!< Node of the object
			FBXSDK_NAMESPACE::FbxNode* fbxNode_;

	};

}	// namespace aqua

