#ifndef SKELETON_H
#define SKELETON_H

namespace FBXSDK_NAMESPACE {
class FbxNode;
class FbxMesh;
class FbxTime;
class FbxAnimStack;
};


namespace aqua {
class SkeletonData;
};

template <class M>
class SkeletonTemplate
{
public:
	typedef M MatrixType;

private:
	/**
	 * header with useful info
	 */
	struct SkeletonHeader
	{
		aqua::Uint32 skeletonId_;
		aqua::Uint32 boneCount_;

		SkeletonHeader(const aqua::Uint32 skeletonId, const aqua::Uint32 boneCount) :
			skeletonId_(skeletonId), boneCount_(boneCount)
		{
		}
	};

	/**
	 * actual transforms
	 */
	struct BoneData
	{
		std::string boneName_;
		MatrixType baseTransform_;
		MatrixType inverseTransform_;

		BoneData(const std::string& boneName, const MatrixType& matrix)
			: boneName_(boneName), baseTransform_(matrix)
		{
			// TO DO -- may need to calc inverse here.
		}
	};

	struct VertexData
	{
		aqua::Array< aqua::Array< aqua::Uint32 > > indices_;
	};


protected:

	/**
	 * info about this skeleton
	 */
	SkeletonHeader skeletonHeader_;

	/**
	 * Array of bones  (CBone)
	 */
	aqua::Array<BoneData> bones_;


	/**
	 * Array of parent bone indices
	 */
	aqua::Array<aqua::Int32> parents_;

	/**
	 * Array of bone indices (hardskin influences)
	 */
	VertexData influences_;

public:
	SkeletonTemplate(const aqua::Uint32 skeletonId, const aqua::Uint32 boneCount) :
		skeletonHeader_(SkeletonTemplate::SkeletonHeader(skeletonId, boneCount))
	{
		bones_.reserve(boneCount);
		influences_.indices_.resize(boneCount);
	}

	aqua::Uint32 reserve(const aqua::Uint32 boneCount)
	{
		PBASSERT(boneCount >= bones_.size(), "Cannot shrink skeleton");
		skeletonHeader_.boneCount_ = boneCount;
		bones_.reserve(boneCount);
		return boneCount;
	}

	const bool addBone(const std::string& boneName, const MatrixType& boneMatrix, aqua::Int32 parent)
	{
		PBASSERT(bones_.size() < skeletonHeader_.boneCount_, "Too many bones for skeleton");
		PBASSERT(parents_.size() < skeletonHeader_.boneCount_, "Too many bones for skeleton");

		bones_.pushBack(BoneData(boneName, boneMatrix));
		parents_.pushBack(parent);
		return true;
	}

	const bool addInfluence(aqua::Uint32 vertex, aqua::Uint32 bone)
	{
		influences_.indices_[bone].pushBack(vertex);
		return true;
	}
		
	const aqua::Array< aqua::Uint32 >& getInfluences(aqua::Uint32 bone) const
	{
		return influences_.indices_[bone];			
	}
	const MatrixType& operator[](aqua::Uint32 index) const
	{
		return bones_[index].baseTransform_;			
	}
		
	const MatrixType& boneMatrix(aqua::Uint32 index) const
	{
		return bones_[index].baseTransform_;			
	}
	const std::string& name(aqua::Uint32 index) const
	{
		return bones_[index].boneName_;			
	}
	const aqua::Int32 parent(aqua::Uint32 index) const
	{
		return parents_[index];			
	}
	const aqua::Uint32 boneCount() const
	{
		return bones_.size();			
	}
};


typedef SkeletonTemplate<FbxAMatrix> fbxSkeleton;
class AnimationTrackContainer;
class AnimationClipContainer;
class AnimationContainer;
class Mesh;

class SkeletonMesh {

public:
	// how long in seconds one frame takes
	static const double frameDuration;

	// time gap between keys
	static const double sampleDuration;

	/**
	 * Test to see if mesh has a skeleton
	 * @param pMesh mesh to test
	 * @return true if something found
	 */
	static bool hasSkeleton(FBXSDK_NAMESPACE::FbxMesh* pMesh);

	/**
	 * Extract skeleton and animation associated with this mesh
	 * @param pMesh Mesh to extract from
	 * @return true if this was animated
	 */
	static aqua::shared_ptr<SkeletonMesh> extractSkeleton(FBXSDK_NAMESPACE::FbxMesh* pMesh);

	
	/**
	 * Extract animations for the skeleton.
	 * @param parser Parser (scene) to use
	 * @param pMesh	 Animatied mesh to extract animations from
	 * @return number of animations extracted
	 */
	aqua::Uint32 extractAnimation(FbxParse *parser, FBXSDK_NAMESPACE::FbxMesh *pMesh);

	boost::shared_ptr<const fbxSkeleton> getSkeleton() const {
		return skeleton_;
	};

	boost::shared_ptr<const AnimationClipContainer > getAnimations() const {
		return animations_;
	};
	
private:

	/** The skeleton. **/
	boost::shared_ptr< fbxSkeleton > skeleton_;

	/** clusters_ aka bones **/
	FbxArray< FbxCluster* > clusters_;

	/** animations_: animations associated with this clip and skeleton **/
	boost::shared_ptr< AnimationClipContainer > animations_;

	/**
	 * Description for ExtractAnimation.
	 * @param tracks tracks that contain the extracted animations
	 * @param pNode Node that we will sample the animations of
	 * @param start Start time of animation sampling
	 * @param end End time of animation sampling
	 */
	void extractTrack(AnimationTrackContainer&				  tracks,
					  FBXSDK_NAMESPACE::FbxMesh*		  pMesh,
					  FBXSDK_NAMESPACE:: FbxCluster*	  pCluster,
					  FBXSDK_NAMESPACE::FbxTime&			  start, 
					  FBXSDK_NAMESPACE::FbxTime&			  end);

	bool extractAnimationTracks(FbxParse* parser, 
								FBXSDK_NAMESPACE::FbxMesh* pMesh, 
								FBXSDK_NAMESPACE::FbxAnimStack* animStack,
								FBXSDK_NAMESPACE::FbxTime& start, 
								FBXSDK_NAMESPACE::FbxTime& stop );

	
};
#endif
