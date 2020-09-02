
#include <StdAfx.h>
#include <PbSystemTypes.h>

#include <aqua/AnimationData.h>

#include "FbxParse.h"
#include "AnimationTimeStamp.h"
#include "AnimationTranslationKey.h"
#include "AnimationRotationKey.h"
#include "AnimationKey.h"
#include "AnimationTrack.h"
#include "AnimationContainer.h"
#include "MetaSerializer.h"
#include "Animation.h"

using namespace aqua;

const double sampleDuration = 1.0 / 120.0;

bool extractAnimationTracks(AnimationTrackContainer& tracks, FbxParse *parser, FbxNode *node) {

	bool result = false;
	
	FbxArray<KString*> animNameArray;				// array of animation names
	KTime period, start, stop, currentTime;
	FbxDynamicArray<FbxAMatrix> globalTransforms;
	KTime sampleRate;
	sampleRate.SetSecondDouble(sampleDuration);
	node->GetScene()->FillAnimStackNameArray(animNameArray);
	int lNbAnimStacks = animNameArray.GetCount();
	// TODO: Possibly test # stacks == 1
	for(int i = 0; i < lNbAnimStacks; ++i) {
		if (animNameArray[i] == NULL)
			continue; // just in case
		result = true;
		parser->setAnimationContext(animNameArray[i], start,stop);
		for(KTime frameTime = start; frameTime <= stop; frameTime += sampleRate)
		{
 			FbxAMatrix globalTRS =  parser->getAnimatedNodeTransform(node, frameTime);
			globalTransforms.PushBack(globalTRS);			
		}
		tracks.openTrack(eTranslationKey);
		// do translation keys
		KTime t = start;
		for(aqua::Uint32 i = 0; i < globalTransforms.GetSize(); ++i)
		{
			AnimationTimeStamp<double> timestamp(t.GetSecondDouble());
	
			FbxVector4 translation( globalTransforms[i].GetT() );
			PbVector3 vec(static_cast<aqua::Float32>(translation[0]), static_cast<aqua::Float32>(translation[1]), static_cast<aqua::Float32>(translation[2]));
			AnimationTranslationTrack::TrackKeyType tkey(timestamp,	AnimationTranslationTrack::AnimationKeyType(translation));
			tracks.addKey<AnimationTranslationTrack>(tkey);
			t += sampleRate;
		}
		tracks.commitTrack("Translation");
		t = start;
		tracks.openTrack(eRotationKey);
		for(aqua::Uint32 i = 0; i < globalTransforms.GetSize(); ++i) {
			AnimationTimeStamp<double> timestamp(t.GetSecondDouble());
			FbxAMatrix		transform = globalTransforms[i];
			FbxQuaternion	kquat  =	transform.GetQ();
			kquat.Normalize();
			AnimationRotationTrack::TrackKeyType rkey(timestamp,  AnimationRotationTrack::AnimationKeyType(kquat));
			tracks.addKey<AnimationRotationTrack>(rkey);
			t += sampleRate;
		}
		tracks.commitTrack("Rotation");
	} // for each anim stack
	return result;
}
