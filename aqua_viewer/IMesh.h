#if !defined(IMESH_H_INCLUDED)
#define IMESH_H_INCLUDED

#include <aqua/TextureSlotData.h>

#if !defined(WIN32)
	bool isNaN(const float) { return x != x; }
	bool isinf(const float x) { return !isNaN(x) && isNaN(x - x); }
#else
	#define isinf(x) (!_finite(x))
#endif

const aqua::Float32 INFINITY = std::numeric_limits<aqua::Float32>::infinity();

/** <  Helper class to hold methods for doing things to intermediate mesh */
class IMesh
{
public:
	struct TextureInfo {
		wxFileName			   fileName;
		wxFileName			   originalName;
		aqua::Float32		   alpha;
		TextureBlendMode	   blendMode;
		aqua::Uint32		   uvSetIndex;
		std::string			   uvSetName;
	};

	
	struct Vertex {
		
		int index_;
		OpenMesh::IntermediateMesh::Point	   vertex_;
		OpenMesh::IntermediateMesh::Normal	   normal_;
		OpenMesh::IntermediateMesh::Color	   color_;
		OpenMesh::IntermediateMesh::TexCoord2D uv1_;
		OpenMesh::IntermediateMesh::TexCoord2D uv2_;

		Vertex(aqua::shared_ptr<OpenMesh::IntermediateMesh> iMesh,	OpenMesh::VPropHandleT<OpenMesh::IntermediateMesh::TexCoord2D>& uv2s, int index) {
			OpenMesh::VertexHandle vh(iMesh->vertex_handle(index));
			index_	= index;
			vertex_ = iMesh->point(vh);
			normal_ = iMesh->has_vertex_normals() ? iMesh->normal(vh) : OpenMesh::IntermediateMesh::Normal(INFINITY, INFINITY, INFINITY);
			color_ =  iMesh->has_vertex_colors() ? iMesh->color(vh) : OpenMesh::IntermediateMesh::Color(INFINITY, INFINITY, INFINITY, INFINITY);
			uv1_   = iMesh->texcoord2D(vh);
			uv2_   = iMesh->property(uv2s, vh);	
		}


		bool proximate(float a, float b) {
			return ((fabs(a-b) < 0.0001f) || (isinf(a) && isinf(b)));
		}

		bool isWeldable(aqua::shared_ptr<OpenMesh::IntermediateMesh> iMesh, OpenMesh::VPropHandleT<OpenMesh::IntermediateMesh::TexCoord2D>& uv2s, int index) {
			if (index == index_)
				return false; //ffs!
			bool result = true;
			OpenMesh::VertexHandle vh(iMesh->vertex_handle(index));
			OpenMesh::IntermediateMesh::Point v	 = iMesh->point(vh);
			// normals (if any)
			OpenMesh::IntermediateMesh::Normal n = iMesh->has_vertex_normals() ? iMesh->normal(vh) : OpenMesh::IntermediateMesh::Normal(INFINITY, INFINITY, INFINITY);
			// colors (if any)
			OpenMesh::IntermediateMesh::Color c = iMesh->has_vertex_colors() ? iMesh->color(vh) : OpenMesh::IntermediateMesh::Color(INFINITY, INFINITY, INFINITY, INFINITY);
			OpenMesh::IntermediateMesh::TexCoord2D	uv1	 = iMesh->texcoord2D(vh);
			OpenMesh::IntermediateMesh::TexCoord2D	uv2 = iMesh->property(uv2s, vh);
			result = proximate(vertex_[0], v[0]) && proximate(vertex_[1], v[1]) && proximate(vertex_[2], v[2]) &&
				proximate(normal_[0], n[0]) && proximate(normal_[1], n[1]) && proximate(normal_[2], n[2]) &&
				proximate(color_[0], c[0]) && proximate(color_[1], c[1]) && proximate(color_[2], c[2]) &&			   
				proximate(uv1_[0], uv1[0]) && proximate(uv1_[1], uv1[1])  &&			   
				proximate(uv2_[0], uv2[0]) && proximate(uv2_[1], uv2[1]);
			return result;
		}
		
	};
   
	typedef std::map< aqua::Uint32, std::vector< TextureInfo > > MaterialTextureDictionary;

	//!
	//! Return the number of uvsets used by the given material
	//!
	static int uvSetCount(MaterialTextureDictionary& materials, int materialIndex);
	
	//!
	//! Create an itermediate mesh and fill it with control point data
	//! @param node Fbx Scene node mesh is attached to
	//! @param mesh Fbx Mesh to extract control points from
	//! @return shared pointer to new immediate mesh
	//!
	static aqua::shared_ptr<OpenMesh::IntermediateMesh> makeIntermediateMesh(KFbxMesh* pMesh);


	/**
	 * Calculate a bounding box around the intermediate mesh
	 * @param iMesh		mesh to calcualate bounds for
	 * @param minExtent minumum extent of box
	 * @param maxExtent maxExtent of box
	 */
	static void calcBoundingBox(aqua::shared_ptr<OpenMesh::IntermediateMesh> iMesh,	 
								aqua::PbVector3& minExtent, 
								aqua::PbVector3& maxExtent);

	//!
	//! Given a vertex buffer format Playbox style, add attributes neeeded to support it to the intermediate mesh
	//! @param iMesh intermediate mesh to fill with attributes
	//! @return true if successful
	//!
	static bool addIntermediateMeshAttributes(aqua::shared_ptr<OpenMesh::IntermediateMesh> iMesh);

	
	//!
	//! Fill in attribute values for intermediate mesh vertices
	//! @param iMesh pointer to intermediate mesh
	//! @param pNode	  Fbx Scene node mesh is attached to
	//! @param pMesh	  Fbx Mesh to use
	//! @param materialIndexes materialIndex per-polygon
	//! @return true if successful
	//!
	static bool fillIntermediateMeshAttributes(aqua::shared_ptr<OpenMesh::IntermediateMesh> iMesh,
											   KFbxNode* pNode,
											   KFbxMesh* pMesh,
											   const std::vector<int>& materialIndexes,
											   MaterialTextureDictionary& textureInfo);

	/**
	 * Add skeleton attributes (wieght, boneindices, original verts to mesh)
	 * @param iMesh pointer to mesh to modify
	 * @param pNode node associated with mesh in scene
	 * @param pMesh mesh attribute of node
	 * @return true if all sparted up well
	 */
	static bool addSkeletonAttributes(aqua::shared_ptr<OpenMesh::IntermediateMesh> iMesh);

	static bool fillSkeletonAttributes(aqua::shared_ptr<OpenMesh::IntermediateMesh> iMesh,
									   KFbxNode* pNode,
									   KFbxMesh* pMesh);
	
private:
	
	static bool fillIntermediateMeshNormals(aqua::shared_ptr<OpenMesh::IntermediateMesh> iMesh,
											KFbxNode* pNode,
											KFbxMesh* pMesh);
	

	static bool fillIntermediateMeshColors(aqua::shared_ptr<OpenMesh::IntermediateMesh> iMesh,
										   KFbxNode* pNode,
										   KFbxMesh* pMesh);

	static bool fillIntermediateMeshFaces(aqua::shared_ptr<OpenMesh::IntermediateMesh> iMesh,
										  KFbxNode* pNode,
										  KFbxMesh* pMesh,
										  const std::vector<int>& materialIndexes,
										  MaterialTextureDictionary& textureInfo);
	
	static bool fillIntermediateMeshUvs(aqua::shared_ptr<OpenMesh::IntermediateMesh> iMesh,
										KFbxNode* pNode,
										KFbxMesh* pMesh,
										const std::vector<int>& materialIndexes,
										MaterialTextureDictionary& textureInfo);

};

#endif

