#include "StdAfx.h"

#include <set>
#include <limits>

// must include gl canvas here so that ogl.h is pulled in after GLee.h
#include <wx/glcanvas.h>

#include "Config.h"
#include "ViewerFrame.h"
#include "IntermediateMesh.h"
#include "SceneManager.h"
#include "Layers.h"
#include "Imesh.h"

using namespace aqua;

/**
 * Create an itermediate mesh and fill it with control point data
 * @param pNode Fbx Scene node mesh is attached to
 * @param pMesh Fbx Mesh to extract control points from
 * @return shared pointer to new immediate mesh
 */
aqua::shared_ptr<OpenMesh::IntermediateMesh> IMesh::makeIntermediateMesh(FbxMesh* pMesh)
{
	// create our imesh
	aqua::shared_ptr<OpenMesh::IntermediateMesh> iMesh(new OpenMesh::IntermediateMesh);
	// get the mesh vertices array
	FbxVector4* meshVertices = pMesh->GetControlPoints();
	// get the Vertex count
	int nVertices = pMesh->GetControlPointsCount();
	// get the polygon count
	int nFaces = pMesh->GetPolygonCount();
	// ensure memory is allocated for this
	iMesh->reserve(nVertices, nFaces * 3, nFaces); // vertices, edges, faces
	// for each vertex
	for (int vertexIndex = 0; vertexIndex < nVertices; ++vertexIndex) {
		FbxVector4 vertex(meshVertices[vertexIndex]);
		iMesh->add_vertex(OpenMesh::IntermediateMesh::Point(aqua::Float32(vertex[0]),
															aqua::Float32(vertex[1]),
															aqua::Float32(vertex[2])));
	}
	// for each polygon we create thee vertex indices (via hanndles)
	for (int polygonIndex = 0; polygonIndex < nFaces; ++polygonIndex) {
		std::vector<OpenMesh::IntermediateMesh::VertexHandle> vHandles;
		// for each point in the polygon
		for (int ipoint = 0; ipoint < pMesh->GetPolygonSize(polygonIndex); ++ipoint) {
			PBASSERT(pMesh->GetPolygonSize(polygonIndex) == 3, "Non triangluar mesh. Found " << pMesh->GetPolygonSize(polygonIndex) << " points.");
			// fish out the vertex index
			int vertexIndex = pMesh->GetPolygonVertex(polygonIndex, ipoint);
			// add it to the face
			PBASSERT((vertexIndex >= 0) && (vertexIndex < nVertices), "Vertex Index out of range.");
			vHandles.push_back(iMesh->vertex_handle(vertexIndex));
		}
		// add the face to the mesh
		iMesh->add_face(vHandles);
	}
	return iMesh;
}

/**
 * Description for calcBoundingBox.
 * @param minExtent minumum value oords of box
 * @param maxExtent maximum value oords of box
 */
void IMesh::calcBoundingBox(shared_ptr<OpenMesh::IntermediateMesh> iMesh,  
							aqua::PbVector3& minExtent, 
							aqua::PbVector3& maxExtent) 

{
	maxExtent.x() = -std::numeric_limits<float>::max();
	maxExtent.y() = -std::numeric_limits<float>::max();
	maxExtent.z() = -std::numeric_limits<float>::max();
	minExtent.x() = std::numeric_limits<float>::max();
	minExtent.y() = std::numeric_limits<float>::max();
	minExtent.z() = std::numeric_limits<float>::max();
	OpenMesh::IntermediateMesh::VertexIter v_it = iMesh->vertices_begin();
	for( ; v_it != iMesh->vertices_end(); ++v_it) {
		OpenMesh::IntermediateMesh::Point v(iMesh->point(v_it));
		if(maxExtent.x() < v[0]) {
			maxExtent.x() = v[0];
		}
		if(maxExtent.y() < v[1]) {
			maxExtent.y() = v[1];
		}
		if(maxExtent.z() < v[2]) {
			maxExtent.z() = v[2];
		}
		if(minExtent.x() > v[0]) {
			minExtent.x() = v[0];
		}
		if(minExtent.y() > v[1]) {
			minExtent.y() = v[1];
		}
		if(minExtent.z() > v[2]) {
			minExtent.z() = v[2];
		}		
	}

}

/**
 * Add skeleton attributes to mesh
 * @param iMesh mesh to add attributes to
 * @return true if successful
 */
bool IMesh::addSkeletonAttributes(shared_ptr<OpenMesh::IntermediateMesh> iMesh) {

	bool result = true;	
	OpenMesh::VPropHandleT<OpenMesh::IntermediateMesh::Point>	  oVerts;
	iMesh->add_property(oVerts, "originalVerts");
	OpenMesh::VPropHandleT<OpenMesh::IntermediateMeshTraits::BoneIndex> iBones;
	iMesh->add_property(iBones, "boneIndexes");
	OpenMesh::VPropHandleT<OpenMesh::IntermediateMeshTraits::Weight>	weights;
	iMesh->add_property(weights, "boneWeights");
	return result;
} 
								  
/**
 * Given a vertex buffer format Playbox style, add attributes neeeded
 * to support it to the intermediate mesh
 * @param iMesh intermediate mesh to fill with attributes
 * @return true if successful
 */
bool IMesh::addIntermediateMeshAttributes(shared_ptr<OpenMesh::IntermediateMesh> iMesh)
{
	bool result = true;
	// first vu map
	iMesh->request_vertex_texcoords2D();
	iMesh->request_halfedge_texcoords2D();
	// second uv map
	OpenMesh::HPropHandleT<OpenMesh::IntermediateMesh::TexCoord2D> huv2s;
	iMesh->add_property(huv2s, "uv2");
	OpenMesh::VPropHandleT<OpenMesh::IntermediateMesh::TexCoord2D> vuv2s;
	iMesh->add_property(vuv2s, "uv2");
	return result;
}

/**
 * Given an intermediate mesh, fill it with normals from the FBX Mesh
 * @param iMesh Mesh to fill
 * @param pNode FbxNode of mesh
 * @param pMesh FbxMesh
 * @return true if successful
 */
bool IMesh::fillIntermediateMeshNormals(shared_ptr<OpenMesh::IntermediateMesh> iMesh,
										FbxNode* pNode,
										FbxMesh* pMesh) {

	PBUNREFERENCED_PARAMETER(pNode);
	bool result = false;

	// vertex normals (will only work if artists use "Split Per Vertex Normals" at export time)
	LayerElementAccessor<FbxLayerElementNormal, FbxVector4> normalElements(pMesh, 0, FbxLayerElement::eNormal, false);
	result = normalElements.isValid();
	if (result) {
		wxLogVerbose("Has normals.");
		iMesh->request_vertex_normals();
		if (!iMesh->has_vertex_normals())
			result = false;
	} else
		wxLogVerbose("Has no normals.");
	// normals
	if (iMesh->has_vertex_normals()) {
		// begin vertex walk
		OpenMesh::IntermediateMesh::VertexIter v_it;
		v_it = iMesh->vertices_begin();
		// set up access to normals
		//	LayerElementAccessor<FbxLayerElementNormal, FbxVector4> normalElements(pMesh, 0, FbxLayerElement::eNORMAL, false);	
		PBASSERT(normalElements.isValid(), "No normal layer, needed by vbuffer.");
		wxLogVerbose("filling in normals");
		// for each vertex, get the normal
		for (int iPoint = 0; iPoint < pMesh->GetControlPointsCount(); ++iPoint) {
			FbxVector4 normal(normalElements.at(iPoint));
			iMesh->set_normal(v_it,
							  OpenMesh::IntermediateMesh::Normal(aqua::Float32(normal[0]),
																 aqua::Float32(normal[1]),
																 aqua::Float32(normal[2])));
			++v_it;
		}
		PBASSERT(v_it == iMesh->vertices_end(), "Mesh vertex count mismatch");
	}
	return result;
}

/**
 * Given an intermediate mesh, fill it with vertex colours from the fbx mesh
 * @param iMesh Intermediate mesh to fill
 * @param pNode Node of mesh 
 * @param pMesh FbxMesh
 * @return true if successful
 */
bool IMesh::fillIntermediateMeshColors(shared_ptr<OpenMesh::IntermediateMesh> iMesh,
									   FbxNode* pNode,
									   FbxMesh* pMesh) {

	PBUNREFERENCED_PARAMETER(pNode);
	
	bool result = false;
	int polygonCount = pMesh->GetPolygonCount();
	
	// if mesh has vertex colours
	LayerElementAccessor<FbxLayerElementVertexColor, FbxColor> colorElements(pMesh, 0, FbxLayerElement::eVertexColor, false);
	result = colorElements.isValid();
	if (result) {
		wxLogVerbose("Has colours.");
		iMesh->request_vertex_colors();
		if (!iMesh->has_vertex_colors())
			result = false;
	} else
		wxLogVerbose("Has no colours.");
	if (iMesh->has_vertex_colors()) {
		OpenMesh::IntermediateMesh::FaceIter f_it = iMesh->faces_begin();
		for (int iface = 0; iface < polygonCount; ++iface) {
			// walk each vertex in the face
			OpenMesh::IntermediateMesh::FaceVertexIter fv_it = iMesh->fv_iter(f_it.handle());
			for (int ipoint = 0; ipoint < pMesh->GetPolygonSize(iface); ++ipoint) {
				FbxColor color(colorElements(iface, ipoint));
				iMesh->set_color(fv_it.handle(),
								 OpenMesh::IntermediateMesh::Color(
									 aqua::Float32(color.mRed),
									 aqua::Float32(color.mGreen),
									 aqua::Float32(color.mBlue),
									 aqua::Float32(color.mAlpha)));
				++fv_it;
			}
			++f_it;
		}
		PBASSERT(f_it == iMesh->faces_end(), "Mesh vertex count mismatch");
	}
	return result;
}

bool IMesh::fillIntermediateMeshFaces(shared_ptr<OpenMesh::IntermediateMesh> iMesh,
									  FbxNode* pNode,
									  FbxMesh* pMesh,
									  const std::vector<int>& materialIndexes,
									  MaterialTextureDictionary& textureInfo)
{
	PBUNREFERENCED_PARAMETER(textureInfo);
	PBUNREFERENCED_PARAMETER(pNode);
	
	// iterate over the materials actually used by the mesh
	int polygonCount = pMesh->GetPolygonCount();
	iMesh->request_face_texture_index();
	bool result = iMesh->has_face_texture_index();
	wxLogDebug("Face Count %d ", iMesh->n_faces());
	if ((true == result) && (false == materialIndexes.empty())) {
		OpenMesh::IntermediateMesh::FaceIter f_it = iMesh->faces_begin();
		for (int polygonIndex = 0; polygonIndex < polygonCount - 1; ++polygonIndex) {
			int materialIndex = materialIndexes[polygonIndex];
			iMesh->set_texture_index(f_it, materialIndex);
			++f_it;
			PBASSERT(f_it != iMesh->faces_end(), "Mesh vertex count mismatch: Expected " << iMesh->n_faces() << ", got " << polygonCount);
		}
	} else {
		wxLogError("Unable to allocate face textures");
	}
	return result;
}

typedef LayerElementAccessor<FbxLayerElementUV, FbxVector2> UVLayerAccessor;

bool IMesh::fillIntermediateMeshUvs(shared_ptr<OpenMesh::IntermediateMesh> iMesh,
									FbxNode* pNode,
									FbxMesh* pMesh,
									const std::vector<int>& materialIndexes,
									MaterialTextureDictionary& textureInfo) {

	PBUNREFERENCED_PARAMETER(pNode);
	PBUNREFERENCED_PARAMETER(materialIndexes);
	PBUNREFERENCED_PARAMETER(textureInfo);
	
	bool result = true;
	int polygonCount = pMesh->GetPolygonCount();

	// clear out uvs to Nan
	OpenMesh::IntermediateMesh::VertexIter v_it;
	v_it = iMesh->vertices_begin();
	OpenMesh::HPropHandleT<OpenMesh::IntermediateMesh::TexCoord2D> uv2s;
	iMesh->get_property_handle(uv2s,"uv2");

	// set all uvs to nan (invalid uv)
	OpenMesh::IntermediateMesh::FaceIter f_it;
	OpenMesh::IntermediateMesh::FaceHalfedgeIter fh_it;
	for (f_it = iMesh->faces_begin(); f_it != iMesh->faces_end(); ++f_it) {
		for (fh_it = iMesh->fh_iter(f_it.handle()); fh_it; ++fh_it) {
			iMesh->set_texcoord2D(fh_it, OpenMesh::IntermediateMesh::TexCoord2D(INFINITY, INFINITY));
			iMesh->property(uv2s, fh_it) = OpenMesh::IntermediateMesh::TexCoord2D(INFINITY, INFINITY);
		}
	}

	shared_ptr<UVLayerAccessor> uv1Layer(new UVLayerAccessor(pMesh,	 0, FbxLayerElement::eTextureDiffuse, true));
	shared_ptr<UVLayerAccessor> uv2Layer(new UVLayerAccessor(pMesh, 1,	FbxLayerElement::eTextureDiffuse, true));		

	f_it = iMesh->faces_begin();
	for (int iface = 0; iface < polygonCount; ++iface) {
		// figure out the face material
		//aqua::Uint32 materialIndex = iMesh->texture_index(f_it);
		// is it this material?
		if (uv1Layer->isValid()) {
			// walk each vertex in the face
			int ipoint(0);
			for (fh_it = iMesh->fh_iter(f_it.handle()); fh_it; ++fh_it) {
				FbxVector2 uv = uv1Layer->operator()(iface, ipoint);
				iMesh->set_texcoord2D(fh_it.handle(),
									  OpenMesh::IntermediateMesh::TexCoord2D(
										  aqua::Float32(uv[0]),
										  aqua::Float32(uv[1])));
				++ipoint;
			}
		}
		if (uv2Layer->isValid()) {
			int ipoint(0);
			for (fh_it = iMesh->fh_iter(f_it.handle()); fh_it; ++fh_it) {
				FbxVector2 uv = uv2Layer->operator()(iface, ipoint);
				OpenMesh::IntermediateMesh::TexCoord2D oldUv = iMesh->property(uv2s, fh_it.handle());
				iMesh->property(uv2s, fh_it.handle()) =
					OpenMesh::IntermediateMesh::TexCoord2D(
					aqua::Float32(uv[0]),
					aqua::Float32(uv[1]));
				++ipoint;
			}
		}
		++f_it;
	}
	return result;
}


/**
 * Description for fillIntermediateMeshAttributes.
 * @param iMesh Intermediate mesh to hold attributes
 * @param pNode FbxNode mesh is attached to in scene
 * @param pMesh Mesh we are going to use for attributes
 * @param materialIndexes materialIndex per-polygon
 * @param textureInfo <doc>
 * @return true if successful
 */
bool IMesh::fillIntermediateMeshAttributes(shared_ptr<OpenMesh::IntermediateMesh> iMesh,
										   FbxNode* pNode,
										   FbxMesh* pMesh,
										   const std::vector<int>& materialIndexes,
										   MaterialTextureDictionary& textureInfo)
{
	bool result = true;
	IMesh::fillIntermediateMeshNormals(iMesh, pNode, pMesh);
	IMesh::fillIntermediateMeshColors(iMesh, pNode, pMesh);
	IMesh::fillIntermediateMeshFaces(iMesh, pNode, pMesh, materialIndexes, textureInfo);
	IMesh::fillIntermediateMeshUvs(iMesh, pNode, pMesh, materialIndexes, textureInfo);
	return result;
}


bool IMesh::fillSkeletonAttributes(shared_ptr<OpenMesh::IntermediateMesh> iMesh,
								   FbxNode* pNode,
								   FbxMesh* pMesh)
{
	PBUNREFERENCED_PARAMETER(pNode);
	
	bool result = false;
	
	if ((pMesh->GetDeformer(0, FbxDeformer::eSkin)) == 0)
	{
		wxLogDebug(wxT("No skin"));
		return result;
	}

	// Count the number of clusters (1 per bone)
	int lClusterCount =( (FbxSkin *)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetClusterCount();

	OpenMesh::VPropHandleT<OpenMesh::IntermediateMesh::Point> ovs;
	iMesh->get_property_handle(ovs,"originalVerts");
	OpenMesh::VPropHandleT<OpenMesh::IntermediateMeshTraits::BoneIndex> iBones;
	iMesh->get_property_handle(iBones, "boneIndexes");
	OpenMesh::VPropHandleT<OpenMesh::IntermediateMeshTraits::Weight> weights;
	iMesh->get_property_handle(weights, "boneWeights");
	
	for(int i = 0; i < lClusterCount; i++)
	{
		FbxCluster* pCluster =((FbxSkin *) pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(i);
		if (!pCluster) {
			wxLogDebug("Skipping empty cluster");
			continue;
		}
		FbxNode* pLink = pCluster->GetLink();
		if (!pLink) {
			wxLogDebug(wxT("Cluster %d is not linked"), i);
			continue;
		}
		
		int lVertexIndexCount = pCluster->GetControlPointIndicesCount();

		// extract weights for this link
		for (int j = 0; j < lVertexIndexCount; ++j)
		{
			int lIndex = pCluster->GetControlPointIndices()[j];
			double lWeight = pCluster->GetControlPointWeights()[j];
#ifdef LOG_VERTEX_WEIGHTS
			wxLogDebug(wxT("Vertex %d Weight %1.12f"), lIndex, lWeight);
#endif
#ifndef SOFTSKINNING
			{
				if (!((lWeight > 0.98) || (lWeight < 0.02))) {
					wxLogDebug(wxT("Possibly softskinned vertex %d "), lIndex);
					OpenMesh::VertexHandle vh(iMesh->vertex_handle(lIndex));
					OpenMesh::IntermediateMesh::Point v	 = iMesh->point(vh);
					iMesh->property(ovs, vh) = v;
					OpenMesh::IntermediateMeshTraits::BoneIndex bi(OpenMesh::IntermediateMeshTraits::BoneIndex(aqua::Uint8(i), 0, 0));
					iMesh->property(iBones, vh) = bi;
					OpenMesh::IntermediateMeshTraits::Weight w(OpenMesh::IntermediateMeshTraits::Weight(aqua::Float32(lWeight), 0.0f, 0.0f));												  	 
					iMesh->property(weights, vh) = w;
				}

			}
#else
			{
				// stub for softskinning
				wxLogError("Soft skinning not supported");
			}
#endif			
		}	
	}
	return true;
}


