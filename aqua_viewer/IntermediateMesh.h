#if !defined(INTERMEDIATEMESH_H_INCLUDED)
#define INTERMEDIATEMESH_H_INCLUDED

// open mesh templates generate lots of these
#pragma warning( disable : 4702 )
#pragma warning( disable : 4127 )

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#pragma warning( default : 4702 )
#pragma warning( default : 4127 )

namespace OpenMesh
{
	struct IntermediateMeshTraits : public OpenMesh::DefaultTraits {
		typedef OpenMesh::Vec3f  Weight;
		typedef OpenMesh::Vec4f  Color;
		typedef OpenMesh::Vec4uc BoneIndex;
		typedef unsigned long    uvChannelMask;

		static bool channelEnabled(uvChannelMask mask, int channel) {
			return ((mask & (1 << channel)) != 0);
		}

		static void enableChannel(uvChannelMask* mask, int channel, bool flag = true) {
			if (flag)
				*mask |=  (1 << channel);
			else
				*mask &= ~(1 << channel);
		}
	};
	
	typedef TriMesh_ArrayKernelT<IntermediateMeshTraits>  IntermediateMesh;
}



#endif
