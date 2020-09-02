#ifndef MESHOBJECT_H
#define MESHOBJECT_H

#include <vector>
#include <string>

#include <PbSystemTypes.h>
#include <Types.h>

class FbxParse;

namespace FBXSDK_NAMESPACE {
	class FbxNode;
};

namespace aqua {

	class MeshObject : public GraphicObject {

	public:
		/**
		 * Constructor
		 * @param parser <doc>
		 * @param node <doc>
		 * @param metaData <doc>
		 */
		MeshObject(FbxParse* parser, FBXSDK_NAMESPACE::FbxNode* node, shared_ptr<DynamicMetaData> metaData);
		
		/**
		 * Return ids of any metadata associated with this mseh
		 * @param ids <doc>
		 * @return <doc>
		 */
		virtual bool ids(std::vector<crc32>& ids);

		/**
		 * Return type of GraphicObjec
		 * @return <doc>
		 */
		virtual Rtti type() const;

		/**
		 * Intiialise any components associated with this object
		 * @param metaData        MetaData object component will be stored in
		 * @param componentName   Name of Component Object
		 * @param componentTypeId Id of Component Type associated with object
		 * @return true if successful
		 */
		virtual bool initialiseComponent(DynamicMetaData& metaData,	 const std::string& componentName, aqua::Uint32 componentTypeId) const;

		/**
		 * Draw the mesh
		 */
		virtual void render(void) const;

		//! return the minimum extent of the bounding box
		virtual const PbVector3 getMinExtent() const;

		//! return the maximum extent of the bounding box
		virtual const PbVector3 getMaxExtent() const;
		
	private:
		std::vector<crc32> vertexBuffers_;
		std::vector<crc32> indexBuffers_;
		std::vector<crc32> materials_;
		std::vector<crc32> textures_;

		//!< Bounding box extents
        aqua::PbVector3 minExtent_;
        aqua::PbVector3 maxExtent_;

	};
}


std::string stripTextureFileName(const std::string& textureFilenme);
	
bool getTextureInformation(FbxNode *pNode, FbxSurfaceMaterial *fbxMaterial, std::vector<IMesh::TextureInfo>& materialTextureInfo);
bool validateTexture(FbxNode* pNode, wxImage& textureImage, bool& textureHasAlpha, std::vector<IMesh::TextureInfo>::iterator ti_it);

aqua::Byte*  textureData(const wxImage& image, aqua::Uint32& size);
	
#endif
