
#include "StdAfx.h"

#include <Utils/HashString.h>
#include <aqua/AnimationData.h>

#include "AnimationTimeStamp.h"
#include "AnimationTranslationKey.h"
#include "AnimationRotationKey.h"
#include "AnimationKey.h"
#include "AnimationTrack.h"
#include "AnimationContainer.h"

using namespace aqua;

// -- tracks ------------
AnimationTrackContainer::AnimationTrackContainer() 
{
		
};

AnimationTrackContainer::~AnimationTrackContainer()
{
	// to do, iterate over and destroy all tracks
}

bool AnimationTrackContainer::trackExists(const std::string& name) const
{
	return (find(trackNames_.begin(), trackNames_.end(), name) != trackNames_.end());
}

AnimationTrackPointer AnimationTrackContainer::track(Uint32 index) const
{
	return tracks_[index];
}

const std::string& AnimationTrackContainer::trackName(Uint32 index) const
{
	return trackNames_[index];
}

aqua::Uint32 AnimationTrackContainer::trackIndex(const std::string& name) const
{
	ConstTrackNameIterator it(find(trackNames_.begin(), trackNames_.end(), name));
	PBASSERT(it != trackNames_.end(), "Unknown track");
	return it - trackNames_.begin();
}

aqua::Uint32 AnimationTrackContainer::trackCount() const
{
	return tracks_.size();
}

	
const bool AnimationTrackContainer::openTrack(eAnimationKeyType kind)
{
	bool result = false;

	if (!cachedTrack_) {
		cachedTrack_ = AnimationTrackFactory::newTrack(kind);
	}		
	return result;
}

const bool AnimationTrackContainer::closeTrack()
{
	bool result = false;

	if (cachedTrack_) {
		result = true;
		cachedTrack_.reset();
	}
	return result;
}

const bool AnimationTrackContainer::commitTrack(const std::string& name)
{
	bool result = false;
		
	if ((cachedTrack_) &&
		(!trackExists(name)))			
	{
		cachedTrack_.sort();
		tracks_.push_back(cachedTrack_);
		trackNames_.push_back(name);
		result = closeTrack();		
	}
	return result;
}

// --- animations -------------------	
const bool AnimationContainer::boneExists(const std::string& name) const
{
	ConstBoneNameIterator it = find(boneNames_.begin(), boneNames_.end(), name);
	return (it != boneNames_.end());		
}

const aqua::Uint32 AnimationContainer::boneCount() const
{
	return bones_.size();
}
	
const bool AnimationContainer::addBone(const std::string& name, const AnimationTrackContainer& tracks)
{
	bool result = false;
	if (!boneExists(name))
	{
		bones_.push_back(tracks);
		boneNames_.push_back(name);
		result = true;
	}
	return result;
}

const AnimationTrackContainer& AnimationContainer::bone(Uint32 i) const
{
	return bones_[i];
}

const std::string&  AnimationContainer::boneName(Uint32 i) const
{
	return boneNames_[i];
}

const AnimationTrackContainer& AnimationContainer::boneTracks(const std::string& name)
{
	ConstBoneNameIterator it = find(boneNames_.begin(), boneNames_.end(), name);	
	wxASSERT(it != boneNames_.end());
	return bones_[it - boneNames_.begin()];
}

const AnimationTrackContainer& AnimationContainer::boneTracks(const std::string& name) const
{
	ConstBoneNameIterator it = find(boneNames_.begin(), boneNames_.end(), name);	
	wxASSERT(it != boneNames_.end());
	return bones_[it - boneNames_.begin()];
}


// -- clips -------------------------------
	
aqua::Uint32 AnimationClipContainer::addClip(const std::string& clipName, AnimationContainer& container)
{
	container.name_ = clipName;
	aqua::Uint32 result = clips_.size();
	clips_.push_back(container);
	return result;
}

const AnimationContainer* AnimationClipContainer::getClip(aqua::Uint32 index) const
{
	const AnimationContainer *result = NULL;
	if (index < clips_.size())
		result = &(clips_.at(index));
	return result;
}
