#ifndef IGRAPHICOBJECT_H
#define IGRAPHICOBJECT_H

#include <vector>
#include "Types.h"
#include "Rtti.h"

class SceneManager;
class DynamicMetaData;
class FbxParse;

namespace FBXFILESDK_NAMESPACE {
	class KFbxNode;
};

namespace aqua {

	class HashString;

	// Iterface that external classes should be using to interrogate game object so it can
	// be shared between different rreenderers without the rest of the application throwing a fit!
	class IGraphicObject {

			friend class SceneManager;

		public:
			/**
			 * Return a pointer to the transform matrix
			 * @return <doc>
			 */
			virtual shared_ptr< const PbMatrix4 > getTransformMatrixPtr() const =  0;

			/**
			 * Return a pointer to the transform matrix
			 * @return <doc>
			 */
			virtual shared_ptr< const PbMatrix4 > getRelativeTransformMatrixPtr() const =  0;

			//! return pointer to the scene transform of this objects parent
			virtual shared_ptr< const PbMatrix4 > getParentTransformMatrixPtr() const = 0;
			
			/**
			 * return the scene transform of this object
			 * @return Transform as 4x4 matrix
			 */
			virtual PbMatrix4 transform() = 0;


			/**
			 * set the position of this object in the scene
			 * @param x Position x coord
			 * @param y Position y coord
			 * @param z Potition z coord
			 */
			virtual void setPosition(PbReal x, PbReal y, PbReal z) = 0;

			/**
			 * return the position of this object in the scene
			 * @param x Position x coord reference
			 * @param y Position y coord reference
			 * @param z Potition z coord reference
			 */
			virtual void getPosition(PbReal& x, PbReal& y, PbReal& z) const = 0;

			/**
			 * return the name of the object
			 * @return Name as std::string
			 */
			virtual const std::string& getObjectName() const = 0;

			/**
			 * set the name of the object
			 * @param objectName name as std::string
			 */
			virtual void setObjectName( const std::string& objectName ) = 0;

			/**
			 * get the name of the object
			 * @return Name as hash string
			 */
			virtual const aqua::HashString& getName() const = 0;

			/**
			 * get the hint associated with the object
			 * @return Hint as std::string
			 */
			virtual const std::string& getHint() const = 0;

			/**
			 * Get the name of the parent
			 * @return name of parent as hash string
			 */
			virtual aqua::HashString getParent() const = 0;

			/**
			 * get the name of the parent object
			 * @return Name as string
			 */
			virtual const std::string& getParentName() const = 0;

			/**
			 * add a child node
			 * @param child name of child as HashString
			 */
			virtual void  addChild(aqua::HashString child) = 0;

			/**
			 * get name of an indexed child
			 * @param n Index of child
			 * @return Name of child as hash string
			 */
			virtual const aqua::HashString& getChild(int n) const = 0;

			/**
			 * Get number of children
			 * @return Number of childern
			 */
			virtual int nChildren() const = 0;


			/**
			 * Convert self to metadata
			 * @param meta <doc>
			 */
			virtual void serialiseSelf(DynamicMetaData& meta) const = 0;

			/**
			 * Convert read self from metadata
			 * @param meta <doc>
			 */
			virtual void deserialiseSelf(DynamicMetaData& meta) = 0;

			/**
			 * Identify the type of the object (silly really, as we can afford real Rtti in tools)
			 * @return Type as rtti object
			 */
			virtual Rtti type() const = 0;

			// metadata query
			virtual bool ids(std::vector<crc32>& ids) = 0;

			/**
			 * initialsie a component based on the data of this object
			 * @param metaData <doc>
			 * @param componentName <doc>
			 * @param componentTypeId <doc>
			 * @return <doc>
			 */
			virtual bool initialiseComponent(DynamicMetaData& metaData,  const std::string& componentName, aqua::Uint32 componentTypeId) const = 0;


			//! return the minimum extent of the bounding box
			virtual const PbVector3 getMinExtent() const = 0;

			//! return the maximum extent of the bounding box
			virtual const PbVector3 getMaxExtent() const = 0;

			/** Actually draw the thing */
			virtual void render() const = 0;


		private:
			virtual shared_ptr< PbMatrix4 > getTransformMatrixPtr()  =  0;

	};

}

#endif

