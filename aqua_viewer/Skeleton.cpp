#include "StdAfx.h"

#include <aqua/VertexBufferData.h>
#include <aqua/BoneData.h>
#include <aqua/SkeletonData.h>
#include <aqua/AnimationData.h>

#include <limits>

#include "FbxParse.h"
#include "AnimationTimeStamp.h"
#include "AnimationTranslationKey.h"
#include "AnimationRotationKey.h"
#include "AnimationKey.h"
#include "AnimationTrack.h"
#include "AnimationContainer.h"
#include "MetaSerializer.h"
#include "Skeleton.h"

using namespace aqua;

const double SkeletonMesh::sampleDuration = 1.0 / 120.0;
const double SkeletonMesh::frameDuration  = 1.0 / 30.0;

// serialise a skeleton to metadata
template<> void MetaSerialiser<SkeletonMesh>(const SkeletonMesh& data, DynamicMetaParser& parser) {
    PBASSERT(parser.IsValid(), "Invalid parser");
    boost::shared_ptr<const fbxSkeleton> skeleton(data.getSkeleton());
    DynamicMetaParser bonesParser(parser.Find("bones_"));
    PBASSERT(bonesParser.IsValid(), "Invalid parser");
    bonesParser.SetArrayCount(skeleton->boneCount());
    DynamicMetaParser parentsParser(parser.Find("parents_"));
    PBASSERT(parentsParser.IsValid(), "Invalid parser");
    parentsParser.SetArrayCount(skeleton->boneCount());
    for(Uint32 bonei = 0; bonei < skeleton->boneCount(); ++bonei) {
        DynamicMetaParser boneParser(bonesParser.Find(bonei, MetaParser::FIND_BY_INDEX));
        boneParser.Find("boneId_").SetValue(aqua::HashString(skeleton->name(bonei).c_str()));
        FbxAMatrix boneMatrix(skeleton->boneMatrix(bonei));
        aqua::shared_ptr<aqua::PbMatrix4> bm(fbxToPlayboxTransform(boneMatrix));
        FbxAMatrix inverseBoneMatrix(boneMatrix.Inverse());
        aqua::shared_ptr<aqua::PbMatrix4> ibm(fbxToPlayboxTransform(inverseBoneMatrix));
        DynamicMetaParser transformParser(boneParser.Find("baseTransform_"));
        for(Uint32 i = 0; i < 16; ++i) {
            transformParser.Find(i, MetaParser::FIND_BY_INDEX).SetValue(bm->data()[i]);
        }
        DynamicMetaParser inverseTransformParser(boneParser.Find("inverseTransform_"));
        for(Uint32 i = 0; i < 16; ++i) {
            inverseTransformParser.Find(i, MetaParser::FIND_BY_INDEX).SetValue(ibm->data()[i]);
        }
        parentsParser.Find(bonei, MetaParser::FIND_BY_INDEX).SetValue(skeleton->parent(bonei));
    }
}

// serialise a set of animation clips to metadata
template<>	void MetaSerialiser<AnimationClipContainer>(const AnimationClipContainer& data, DynamicMetaParser& parser)
{
    // serialise a clip container.
    Uint32 clipTypeCRC = CalcCRC("AnimationClip");
    Uint32 jointTypeCRC = CalcCRC("AnimationJoint");
    Uint32 trackTypeCRC = CalcCRC("AnimationTrack");
    PBASSERT(parser.IsValid(), "Invalid parser");
    DynamicMetaData* metaData = parser.GetDynamicMetaData();
    PBASSERT(metaData != NULL, "Invalid metadata");
    Uint32 clipIndex = 0;
    const AnimationContainer* clip = NULL;
    while( (clip = data.getClip(clipIndex)) != NULL ) {
        // serialise a clip
        aqua::HashString clipNameHash(clip->name_.c_str());
        DynamicMetaParser clipParser = metaData->AllocateData(clip->name_.c_str(), clipTypeCRC);
        PBASSERT(clipParser.IsValid(), "Invalid parser");
        PBASSERT(clipParser.Find("startTime_").IsValid(), "Invalid parser!");
        // still start time and end time to do
        clipParser.Find("animationId_").SetValue(clipNameHash);
        clipParser.Find("flags_").SetValue(0);
        std::vector<std::string> jointNames;
        aqua::Float32 startTime  =	std::numeric_limits<aqua::Float32>::max();
        aqua::Float32 endTime    =	-std::numeric_limits<aqua::Float32>::max();
        for(Uint32 jointIndex = 0; jointIndex < clip->boneCount(); ++jointIndex) {
            AnimationTrackContainer joint(clip->bone(jointIndex));
            std::string metaJointName = clip->name_ + std::string("_") + clip->boneName(jointIndex);
            jointNames.push_back(metaJointName);
            for(Uint32 trackIndex = 0; trackIndex < joint.trackCount(); ++trackIndex) {
                AnimationTrackPointer track = joint.track(trackIndex);
                for(Uint32 keyIndex = 0; keyIndex < track.size(); ++keyIndex) {
                    aqua::AnimationKey thisKey;
                    track.key(keyIndex, thisKey);
                    if (thisKey.t < startTime)
                        startTime = thisKey.t;
                    if (thisKey.t > endTime)
                        endTime = thisKey.t;
                } // for each key
            } // for each track
        } // fore each joint
        clipParser.Find("startTime_").SetValue(startTime);
        clipParser.Find("endTime_").SetValue(endTime);
        DynamicMetaParser jointIdParser = clipParser.Find("joints_");
        jointIdParser.SetArrayCount(clip->boneCount());
        for(Uint32 jointIndex = 0; jointIndex < clip->boneCount(); ++jointIndex) {
            aqua::HashString jointId(jointNames[jointIndex].c_str());
            jointIdParser.Find(jointIndex, MetaParser::FIND_BY_INDEX).SetValue(jointId);
        }
        // now, do the joints
        for(Uint32 jointIndex = 0; jointIndex < clip->boneCount(); ++jointIndex) {
            DynamicMetaParser jointParser = metaData->AllocateData(jointNames[jointIndex].c_str(), jointTypeCRC);
            PBASSERT(jointParser.IsValid(), "Invalid parser");
            AnimationTrackContainer joint(clip->bone(jointIndex));
            // prepare to fill in tracks
            std::vector<std::string> trackNames;
            for(Uint32 trackIndex = 0; trackIndex < joint.trackCount(); ++trackIndex) {
                std::string metaTrackName = jointNames[jointIndex] + std::string("_") + joint.trackName(trackIndex);
                trackNames.push_back(metaTrackName);
            }
            DynamicMetaParser trackArray = jointParser.Find("tracks_");
            trackArray.SetArrayCount(trackNames.size());
            for(Uint32 trackIndex = 0; trackIndex < joint.trackCount(); ++trackIndex) {
                aqua::HashString trackId(trackNames[trackIndex].c_str());
                trackArray.Find(trackIndex, MetaParser::FIND_BY_INDEX).SetValue(trackId);
            }
            // now, do the tracks
            for(Uint32 trackIndex = 0; trackIndex < joint.trackCount(); ++trackIndex) {
                DynamicMetaParser trackParser = metaData->AllocateData(trackNames[trackIndex].c_str(), trackTypeCRC);
                PBASSERT(trackParser.IsValid(), "Invalid parser");
                AnimationTrackPointer track = joint.track(trackIndex);
                trackParser.Find("trackType_").SetValue((Uint32) track.kind());
                DynamicMetaParser keyParser = trackParser.Find("keys_");
                PBASSERT(keyParser.IsValid(), "Can't find keys_ member");
                keyParser.SetArrayCount(track.size());
                for(Uint32 keyIndex = 0; keyIndex < track.size(); ++keyIndex) {
                    aqua::AnimationKey thisKey;
                    track.key(keyIndex, thisKey);
                    if (thisKey.t < startTime)
                        startTime = thisKey.t;
                    if (thisKey.t > endTime)
                        endTime = thisKey.t;
                    DynamicMetaParser iKeyParser = keyParser.Find(keyIndex, MetaParser::FIND_BY_INDEX);
                    iKeyParser.Find("t").SetValue( thisKey.t );
                    iKeyParser.Find("x").SetValue( thisKey.x );
                    iKeyParser.Find("y").SetValue( thisKey.y );
                    iKeyParser.Find("z").SetValue( thisKey.z );
                    iKeyParser.Find("w").SetValue( thisKey.w );
                }
            } // for each track
        } // for each joint
        clipIndex++;
    } // for each clip
};

/**
 * Find the parent index of a bone in the skeleton
 * @param index Index of the bone
 * @param skeleton Skeleton to scan
 * @return Index of parent or -1 if not found
 */
int findParent(int index, const FbxArray< FbxCluster* >& skeleton)
{
    int result = -1;

    const FbxCluster* lCluster = skeleton[index];
    if ((!lCluster) || (!lCluster->GetLink()))
    {
        return -1;
    }

    const FbxNode* lLink = lCluster->GetLink();
    if (!lLink)
    {
        return -1;
    }

    const FbxNode* lLinkParent = lLink->GetParent();
    if (!lLinkParent)
    {
        return -1;
    }

    for(int i = 0; i < skeleton.GetCount(); i++)
    {
        if (lLinkParent == skeleton[i]->GetLink())
            result = i;
    }
    return result;
}

void testForScaling(FbxAMatrix& mat)
{

    FbxVector4 kScale = mat.GetS();
    wxASSERT(kScale[0] > 0.9);
    wxASSERT(kScale[0] < 1.1);
    wxASSERT(kScale[1] > 0.9);
    wxASSERT(kScale[1] < 1.1);
    wxASSERT(kScale[2] > 0.9);
    wxASSERT(kScale[2] < 1.1);
}

bool SkeletonMesh::hasSkeleton(FbxMesh* pMesh) {
    wxLogDebug(wxString::Format(wxT("Testing %s for skin "), pMesh->GetName()));
    if ((pMesh->GetDeformer(0, FbxDeformer::eSkin)) == 0)
    {
        wxLogDebug(wxT("No skin"));
        return false;
    }
    // Count the number of clusters (1 per bone)
    int lClusterCount =( (FbxSkin *)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetClusterCount();
    return lClusterCount > 0;
}

aqua::shared_ptr<SkeletonMesh> SkeletonMesh::extractSkeleton(FbxMesh* pMesh)
{

    aqua::shared_ptr<SkeletonMesh> result;

    if ((pMesh->GetDeformer(0, FbxDeformer::eSkin)) == 0)
    {
        wxLogDebug(wxT("No skin"));
        return result;
    }

    // Count the number of clusters (1 per bone)
    int lClusterCount =( (FbxSkin *)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetClusterCount();

    // All the links must have the same link mode.
    FbxCluster::ELinkMode lClusterMode = ((FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();
    if (lClusterMode == FbxCluster::eAdditive) {
        wxLogError(wxT("Additive deformation mode unspported (at present)"));
        return result;
    }

    result = aqua::shared_ptr<SkeletonMesh>(new SkeletonMesh());
    result->clusters_.Empty();
    FbxDynamicArray< FbxAMatrix > localTransforms(lClusterCount);
    FbxDynamicArray< FbxAMatrix > globalTransforms(lClusterCount);
    FbxDynamicArray< int > parents(lClusterCount);

    for(int i = 0; i < lClusterCount; i++)
    {

        FbxCluster* lCluster =((FbxSkin *) pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(i);
        result->clusters_.Add(lCluster);
        if ((!lCluster) || (!lCluster->GetLink())) {
            wxLogDebug(wxT("Adding unlinked cluster %d "), i);
            FbxAMatrix identity;
            localTransforms.PushBack(identity);
            globalTransforms.PushBack(identity);
            continue;
        }

        FbxNode *bone = lCluster->GetLink();
        wxLogDebug(wxT("Processing bone  %s "), wxString(bone->GetName(), wxConvUTF8));
        // get the referenece (and geometry) transforms (bone rest)
        FbxAMatrix lReferenceGlobalInitTransform;
        lCluster->GetTransformMatrix(lReferenceGlobalInitTransform);
//		testForScaling(lReferenceGlobalInitTransform);

        FbxAMatrix lReferenceGeometry = getGeometryTransform(pMesh->GetNode());
        lReferenceGlobalInitTransform *= lReferenceGeometry;
//		testForScaling(lReferenceGeometry);

        // cluster transform (mesh rest transform)
        FbxAMatrix lClusterGlobalInitTransform;
        lCluster->GetTransformLinkMatrix(lClusterGlobalInitTransform);
//		testForScaling(lClusterGlobalInitTransform);

        // compute the bindpose as delta transform
        FbxAMatrix lClusterRelativeInitTransform = lClusterGlobalInitTransform.Inverse() * lReferenceGlobalInitTransform;
        testForScaling(lClusterRelativeInitTransform);

        globalTransforms.PushBack(lReferenceGlobalInitTransform);
        localTransforms.PushBack(lClusterRelativeInitTransform);
    }


    // check all the clusters are parented
    for(int i = 0; i < lClusterCount; i++)
    {
        int parent = findParent(i, result->clusters_);
        if (parent == -1) {
            wxLogDebug(wxT("Bone %s does not have a parent "), wxString(result->clusters_[i]->GetLink()->GetName(), wxConvUTF8));
        } else {
            wxLogDebug(wxT("Bone %s scene parent is "), wxString(result->clusters_[i]->GetLink()->GetParent()->GetName(), wxConvUTF8));

        }
    }

    wxLogDebug(wxT("Creating %d bone skeleton"), lClusterCount);
    result->skeleton_ = aqua::shared_ptr<fbxSkeleton>(new fbxSkeleton(0, lClusterCount));

    // now iterate over the clusters and put the transforms into the pbSkeleton
    for(int i = 0; i < lClusterCount; i++)
    {
		std::string boneName("Unlinked");
        if (result->clusters_[i]->GetLink())
            boneName =	std::string(wxString(result->clusters_[i]->GetLink()->GetName(),wxConvUTF8).utf8_str());

        aqua::PbMatrix4 boneMatrix;
        for(int row = 0; row < 4; row++)
            for(int col = 0; col < 4; col++)
                boneMatrix(row,col) = aqua::Float32(localTransforms[i].GetRow(row)[col]);
        result->skeleton_->addBone(boneName, localTransforms[i], parents[i]); // todo bone id based on name hash
    }


//	lControlPoints = pMesh->GetControlPoints();
    int lDeformerCount = pMesh->GetDeformerCount();
    wxLogDebug(wxT("Mesh has %d deformers"), lDeformerCount);
    FbxSkin *pSkin = (FbxSkin *) pMesh->GetDeformer(0, FbxDeformer::eSkin);
    if (!pSkin) {
        wxLogError(wxT("Non skin Deformer found"));
        return aqua::shared_ptr<SkeletonMesh>();
    }

// extract the weights
    for(int i = 0; i < result->clusters_.GetCount(); i++)
    {
        FbxCluster *pCluster = result->clusters_[i];

        FbxNode* pLink = pCluster->GetLink();
        if (!pLink) {
            wxLogDebug(wxT("Cluster %d is not linked"), i);
            continue;
        }

        wxLogDebug(wxT("Link Name %s "), wxString(pLink->GetName(),wxConvUTF8));

        int lVertexIndexCount = pCluster->GetControlPointIndicesCount();
        wxLogDebug(wxT("Cluster %d influences %d vertices "), i, lVertexIndexCount);
        switch (lClusterMode)
        {
            case FbxCluster::eNormalize:
                wxLogDebug(wxT("Normalized weights."));
                break;
            case FbxCluster::eAdditive:
                wxLogError(wxT("Additive Weights not supported"));
                return aqua::shared_ptr<SkeletonMesh>();
            case FbxCluster::eTotalOne:
                wxLogDebug(wxT("Normal weights."));
                break;
        }

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
                if (!((lWeight > 0.98) || (lWeight < 0.02)))
                    wxLogDebug(wxT("Possibly softskinned vertex %d "), lIndex);
                result->skeleton_->addInfluence(lIndex, i);
            }
#else			
            {
                // stub for softskinning
            }
#endif			
        }
    }

// ok, finished
    return result;
}

/**
 * Description for extractTrack.
 * @param tracks tracks that contain the extracted animations
 * @param pNode Node that we will sample the animations of
 * @param start Start time of animation sampling
 * @param end End time of animation sampling
 */
void SkeletonMesh::extractTrack(AnimationTrackContainer& tracks,
                                FbxMesh*                pMesh,
                                FbxCluster*             pCluster,
                                FbxTime& start,
                                FbxTime& end)
{
    bool errorDetected = false;

    FbxTime sampleRate;
    sampleRate.SetSecondDouble(sampleDuration);


    FbxDynamicArray<FbxAMatrix> globalTransforms;
    FbxDynamicArray<FbxAMatrix> localTransforms;
    FbxTime frameTime;

    FbxAMatrix lGeometryOffset = getGeometryTransform(pMesh->GetNode());

    // compute joint transforms?
    // TODO : This is not right - go back and check
    for(FbxTime frameTime = start; frameTime <= end; frameTime += sampleRate)
    {
        FbxNode   *meshNode = pMesh->GetNode();
        FbxAnimEvaluator *animEval = meshNode->GetScene()->GetEvaluator();
        FbxAMatrix lGlobalPosition = animEval->GetNodeGlobalTransform(meshNode,frameTime);
        FbxAMatrix lReferenceGlobalCurrentPosition = lGlobalPosition * lGeometryOffset;

        FbxNode   *linkNode = pMesh->GetNode();
        FbxAnimEvaluator *linkAnimEval = linkNode->GetScene()->GetEvaluator();
        FbxAMatrix lClusterGlobalCurrentPosition  = linkAnimEval->GetNodeGlobalTransform(linkNode, frameTime);
        FbxAMatrix lClusterRelativeCurrentPositionInverse =  lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;
        globalTransforms.PushBack(lClusterGlobalCurrentPosition);
        localTransforms.PushBack(lClusterRelativeCurrentPositionInverse);
    }

    tracks.openTrack(eTranslationKey);

    // do translation keys
    FbxTime t = start;
    for(aqua::Uint32 i = 0; i < localTransforms.GetSize(); ++i)
    {
        AnimationTimeStamp<double> timestamp(t.GetSecondDouble());

        FbxVector4 translation( localTransforms[i].GetT() );
        AnimationTranslationTrack::TrackKeyType tkey(timestamp,	AnimationTranslationTrack::AnimationKeyType(translation));
        tracks.addKey<AnimationTranslationTrack>(tkey);
        t += sampleRate;
    }
    tracks.commitTrack("Translation");

    // do rotation keys
    t = start;
    tracks.openTrack(eRotationKey);
    errorDetected = false;
    for(aqua::Uint32 i = 0; i < localTransforms.GetSize(); ++i) {

        AnimationTimeStamp<double> timestamp(t.GetSecondDouble());

        FbxAMatrix		transform = localTransforms[i];
        FbxVector4		kTrans =	transform.GetT();
        FbxQuaternion	kquat  =	transform.GetQ();
        FbxVector4		kscale =	transform.GetS();

        // float row2l = row1.Length();

        if ((kscale[0] < 0.9) || (kscale[0] > 1.1) || (kscale[1] < 0.9) || (kscale[1] > 1.1) || (kscale[2] < 0.9) || (kscale[2] > 1.1))
        {
            if (!errorDetected)
            {
                wxLogError(wxT("Current Time %4.8f "), t.GetSecondDouble());
                wxLogError(wxT("Current Pose ========= %s "), wxString(pCluster->GetLink()->GetName(), wxConvUTF8));
                wxLogError(wxT("Scaling Detected!!"));
                errorDetected = true;
            }
        }
        AnimationRotationTrack::TrackKeyType rkey(timestamp,  AnimationRotationTrack::AnimationKeyType(kquat));
        tracks.addKey<AnimationRotationTrack>(rkey);
        t += sampleRate;
    }
    tracks.commitTrack("Rotation");

}


/**
 * Description for extractAnimationTracks.
 * @param parser <doc>
 * @param pMesh <doc>
 * @param animStack <doc>
 * @param start <doc>
 * @param stop <doc>
 * @return <doc>
 */
bool SkeletonMesh::extractAnimationTracks(FbxParse* parser, FbxMesh* pMesh, FbxAnimStack* animStack, FbxTime& start, FbxTime& stop )
{
    PBUNREFERENCED_PARAMETER(animStack);
    PBUNREFERENCED_PARAMETER(parser);
    wxString clipName(wxString::Format(wxT("%s_Clip "), pMesh->GetNode()->GetName()));
    if ((pMesh->GetDeformer(0, FbxDeformer::eSkin)) == 0)
    {
        wxLogDebug(wxString::Format(wxT("No skin on mesh %s "), pMesh->GetName()));
        return false;
    }

    // All the links must have the same link mode.
    FbxCluster::ELinkMode lClusterMode = ((FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();
    if (lClusterMode == FbxCluster::eAdditive) {
        wxLogError(wxT("Additive deformation mode unspported (at present)"));
        return false;
    }

    AnimationContainer animation;
    for(int i = 0; i < clusters_.GetCount(); i++)
    {
        AnimationTrackContainer bone_animation;
        extractTrack(bone_animation, pMesh, clusters_[i], start, stop);
        animation.addBone(clusters_[i]->GetLink()->GetName(), bone_animation);
    }
    animations_->addClip( std::string(clipName.mb_str()), animation);
    return true;
}

/** ExtractAnimation - extract animations from the scene by scanning for gaps . */
Uint32 SkeletonMesh::extractAnimation(FbxParse *parser, FbxMesh *pMesh)
{
    FbxAnimStack* currentAnimationStack;
    KTime period, start, stop, currentTime;
    Uint32	result = 0;
    animations_ = shared_ptr<AnimationClipContainer>(new AnimationClipContainer());
    // get the list of Takes. With Max there is only one, but hey,
    // there's always Blender..
    FbxArray<FbxString*> animNameArray;
    pMesh->GetNode()->GetScene()->FillAnimStackNameArray(animNameArray);
    int lNbAnimStacks = animNameArray.GetCount();
    for(int i = 0; i < lNbAnimStacks; ++i) {
        if (animNameArray[i] == NULL)
            continue; // just in case
        currentAnimationStack = parser->setAnimationContext(animNameArray[i], start,stop);
        if (currentAnimationStack == NULL)
            continue;
        if (extractAnimationTracks(parser, pMesh, currentAnimationStack, start, stop))
            result++;
    }
    return result;
}


