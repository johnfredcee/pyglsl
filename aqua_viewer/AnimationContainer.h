#ifndef ANIMATIONCONTAINER_H
#define ANIMATIONCONTAINER_H



/** This contains a number of animation tracks for the purpose of animating a rather more complex entity
	such as a person or a wombat or a spider or a mech..or..whatever, really. **/
class AnimationTrackContainer
{
public:
	typedef std::vector< typename std::string > TrackNameMap;
	typedef std::vector< typename AnimationTrackPointer > TrackMap;
private:
	TrackMap					tracks_;
	TrackNameMap				trackNames_;
	AnimationTrackPointer		cachedTrack_;

public:
	typedef TrackMap::iterator           TrackIterator;
	typedef TrackMap::const_iterator     ConstTrackIterator;
	typedef TrackNameMap::iterator       TrackNameIterator;
	typedef TrackNameMap::const_iterator ConstTrackNameIterator;
	
	AnimationTrackContainer();

	~AnimationTrackContainer();

	bool trackExists(const std::string& name) const;

	AnimationTrackPointer track(aqua::Uint32 index) const;

	const std::string& trackName(aqua::Uint32 index) const;
		
	aqua::Uint32 trackIndex(const std::string& name) const;
	
	aqua::Uint32 trackCount() const;
	
	const bool openTrack(eAnimationKeyType type);

	const bool closeTrack();

	const bool commitTrack(const std::string& name);

	const bool trackNames(std::vector<std::string>& names);
		
	template <typename T>
	const void addKey(typename T::TrackKeyType& key)
	{
		wxASSERT(key.time() != -1.0);
		static_cast<T*>(cachedTrack_)->push_back(key);
	}

	template <typename T>
	const T& animationTrack(const std::string& name) const
	{
		ConstTrackIterator result = tracks_.find(name);			
		wxASSERT(result != tracks_.end());
		return *(static_cast<const T*>(result->second));
	}

	template <typename T>
	T& animationTrack(const std::string& name) 
	{
		TrackIterator result = tracks_.find(name);			
		wxASSERT(result != tracks_.end());
		return *(static_cast<T*>(result->second));
	}	   
};

// TO DO :: need to get bones in hierarchy order ! 

/** AnimationContainer contains an track per bone - these are the animated joints
    that match up to a bone in the skeleton */
class AnimationContainer 
{
public:
	typedef std::vector<typename AnimationTrackContainer> BoneMap;
	std::string name_;
	
private:
	std::vector<std::string> boneNames_;
	BoneMap					 bones_;

public:
	typedef BoneMap::iterator							BoneIterator;
	typedef BoneMap::const_iterator						ConstBoneIterator;
	typedef std::vector<std::string>::iterator			BoneNameIterator;
	typedef std::vector<std::string>::const_iterator	ConstBoneNameIterator;
	
	const bool boneExists(const std::string& name) const;

	const aqua::Uint32 boneCount() const;
		
	const bool addBone(const std::string& name, const AnimationTrackContainer& channels);

	const std::string& boneName(aqua::Uint32 i) const;

	const AnimationTrackContainer& bone(aqua::Uint32 i) const;
	
	const AnimationTrackContainer& boneTracks(const std::string& name);

	const AnimationTrackContainer& boneTracks(const std::string& name) const;

	const eAnimationKeyType trackKeyType(const std::string& boneName, const std::string& trackName);		
		
};

class AnimationClipContainer
{
public:
	typedef std::vector<typename AnimationContainer> ClipContainer;

private:
	ClipContainer clips_;
public:	
	aqua::Uint32 addClip(const std::string& clipName, AnimationContainer& container);
	const AnimationContainer* getClip(aqua::Uint32 index) const;
};





#endif


