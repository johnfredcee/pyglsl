#ifndef ANIMATION_H
#define ANIMATION_H

namespace FBXSDK_NAMESPACE {
	class FbxNode;
}

class FbxParse;
class AnimationTrackContainer;
  
bool extractAnimationTracks(AnimationTrackContainer& tracks, FbxParse *parser, FBXSDK_NAMESPACE::FbxNode *node);


#endif
