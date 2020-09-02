#ifndef METABUFFERMANAGER_H
#define METABUFFERMANAGER_H

/** MetaBufferManager: Manage Vertex and VertexIndexBuffers in the
 * scene */
class MetaBufferManager : public aqua::Locator::ServiceSingleton<MetaBufferManager>
{
	friend class aqua::Locator::ServiceSingleton<MetaBufferManager>;
public:
	MetaBufferManager();

	virtual ~MetaBufferManager();


	
	/**
	 * Fill the vertex buffers for a given material
	 * @param name Buffer names
	 * @param instancedVertices Duplicated vertex buffer flag
	 * @param indices Indexes associated with buffer (filled also)
	 * @param instancedIndices Duplicate vertex index buffer flag
	 * @param iMesh Intermediate mesh with whole fbx mesh data
	 * @param materialIndex Index of material to fill from
	 * @return pointer to vertex buffer struct filled in
	 */
	aqua::shared_ptr<aqua::VertexBufferData> fillVertexBuffer(const std::string& name,
															  bool instancedVertices,
															  aqua::Array<aqua::IndexData>& indices,
															  aqua::Array<aqua::BoneInfluence>& influences,														  
															  bool instancedIndices,
															  aqua::shared_ptr<OpenMesh::IntermediateMesh> iMesh,
															  int materialIndex = 0);
	
	/**
	 * Check whether the buffers associated with this name exist and set flags accordingly
	 * @param name names of buffers to check
	 * @param instancedVertices Set to true if the vertex buffers are duplicated
	 * @param instancedIndices Set to true if the index buffers are duplicated
	 * @return not sure
	 */
	bool checkBuffers(const std::string&name,
					  bool& instancedVertices,
					  bool& instancedIndices);


	/**
	 * Actually add already filled vertex buffers to the scene
	 * @param name Name of buffers
	 * @param instancedVertices Flag true if this is a duplicate buffer
	 * @param vertices Vertex buffer to use
	 * @param instancedIndices Flag true if this a a vertex index buffer
	 * @param indices Indices to fill vertex index buffer with
	 * @return true if successful
	 */
	bool addBuffers(const std::string& name,
					bool instancedVertices,
					aqua::shared_ptr<aqua::VertexBufferData> vertices,
					bool instancedIndices,
					aqua::Array<aqua::IndexData> indices,
					aqua::Array<aqua::BoneInfluence> influences); 


private:
	void computeIndexRemapping(aqua::shared_ptr<OpenMesh::IntermediateMesh> iMesh,
							   std::vector<int>& oldIndices,
							   std::vector<aqua::IndexData>&	 indexData,
							   int materialIndex,
							   int materialVertexCount);											  

	void computeMaterialStats(aqua::shared_ptr<OpenMesh::IntermediateMesh> iMesh,
							  int  materialIndex,
							  int& materialPolygonCount,
							  int& materialVertexCount,
							  bool& usesUv1,
							  bool& usesUv2);
	
}; // class MetaBufferManager

/**
 * Return CRC of vertex buffer name
 * @param name Name of buffer pair
 * @return CRC of vertex buffer
 */
crc32 vbufferId(const std::string& name);

/**
 * Return CRC of vertex index buffer name
 * @param name Name of buffer pair
 * @return CRC of vertex index buffer
 */
crc32 ibufferId(const std::string& name);

#endif
