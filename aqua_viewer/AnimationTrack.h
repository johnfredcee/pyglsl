#ifndef ANIMATIONTRACK_H
#define ANIMATIONTRACK_H


/**
 * An array of animaiton keys for the purpose of animating one variable
 */
template <typename T, typename K> class AnimationTrack
{
public:

	typedef typename AnimationTrackKey<T, K> TrackKeyType;
	typedef typename TrackKeyType::KeyType AnimationKeyType;
	typedef typename aqua::Array<typename TrackKeyType> TrackKeyContainerType;

	typedef typename TrackKeyContainerType::iterator AnimationKeyIterator;
	typedef typename TrackKeyContainerType::const_iterator ConstAnimationKeyIterator;

private:
	TrackKeyContainerType keys_;

public:
	ConstAnimationKeyIterator begin() const
	{
		return keys_.begin();
	}

	ConstAnimationKeyIterator end() const
	{
		return keys_.end();
	}

	AnimationKeyIterator begin()
	{
		return keys_.begin();
	}

	AnimationKeyIterator end()
	{
		return keys_.end();
	}

	aqua::Uint32 size() const
	{
		return keys_.size();
	}

	bool empty() const
	{
		return keys_.empty();			
	}
	
	AnimationKeyType& key(aqua::Uint32 i)
	{
		return keys_.at(i).key();
	}

	const AnimationKeyType key(aqua::Uint32 i) const
	{
		return keys_.at(i).key();
	}

	const double time(aqua::Uint32 i) const
	{
		return keys_.at(i).time();
	}

	void push_back(const TrackKeyType& key)
	{
		keys_.pushBack(key);
	}


};

/**
 * Specialisation for a track animating a rotation
 **/
class AnimationRotationTrack : public  AnimationTrack<typename AnimationTimeStamp<double>, typename AnimationRotationKey>
{
public:
	const eAnimationKeyType keyType() const
	{
		return eRotationKey;
	}
};


/**
 * Specialisation for a track animating a Translation (ie vector4)
 **/
class AnimationTranslationTrack : public AnimationTrack<typename AnimationTimeStamp<double>, typename AnimationTranslationKey>
{
public:
	const eAnimationKeyType keyType() const
	{
		return eTranslationKey;
	}
};

class AnimationTrackFactory;
	
/** Encapsulates a pointer to an animation track. Somewhat specialised
 * smart pointer semantics to hide the difference between different
 * specialised tracks. **/
class AnimationTrackPointer
{
	friend class AnimationTrackFactory;
private:

	eAnimationKeyType kind_;
	typedef union
	{
		void*					   data_;
		AnimationTranslationTrack* ttrack_;
		AnimationRotationTrack*	   rtrack_;
	} TrackPointer;
	TrackPointer trackPointer_;

public:

	AnimationTrackPointer()
	{
		kind_ = eDuffKey;
		trackPointer_.data_ = 0;
	}

	AnimationTrackPointer(AnimationTranslationTrack* ptr)
	{
		kind_ = eTranslationKey;
		trackPointer_.ttrack_ = ptr;
	}

	AnimationTrackPointer(AnimationRotationTrack* ptr)
	{
		kind_ = eRotationKey;
		trackPointer_.rtrack_ = ptr;
	}

	eAnimationKeyType kind() const
	{
		return kind_;
	}

	AnimationTrackPointer(const AnimationTrackPointer& other)
	{
		kind_ = other.kind_;
		switch (kind_)
		{
			case eTranslationKey:
				trackPointer_.ttrack_ = other.trackPointer_.ttrack_;
				break;
			case eRotationKey:
				trackPointer_.rtrack_ = other.trackPointer_.rtrack_;
				break;
			default:
				break;
		}
	}

	AnimationTrackPointer& operator=(AnimationTranslationTrack* ptr)
	{
		kind_ = eTranslationKey;
		trackPointer_.ttrack_ = ptr;
		return *this;
	}

	AnimationTrackPointer& operator=(AnimationRotationTrack* ptr)
	{
		kind_ = eRotationKey;
		trackPointer_.rtrack_ = ptr;
		return *this;
	}

	void reset()
	{
		kind_ = eDuffKey;
		trackPointer_.data_ = 0;
	}

	aqua::Uint32 size()
	{
		switch (kind_)
		{
			case eRotationKey:
			{
				AnimationRotationTrack* rtrack = trackPointer_.rtrack_;
				return rtrack->size();
			}
			case eTranslationKey:
			{
				AnimationTranslationTrack* ttrack = trackPointer_.ttrack_;
				return ttrack->size();
			}				
		}
		return 0;
	}

	void key(aqua::Uint32 i, aqua::AnimationKey& key)
	{
		switch (kind_)
		{
			case eRotationKey:
			{
				const aqua::Float32 epsilon = 0.001f;
				AnimationRotationTrack* rtrack = trackPointer_.rtrack_;
				FbxQuaternion rKey( rtrack->key(i)	 );
				key.x = static_cast<aqua::Float32>(rKey[0]);
				if (fabs(key.x) < epsilon)
					key.x = 0.0f;
				key.y = static_cast<aqua::Float32>(rKey[1]);
				if (fabs(key.y) < epsilon)
					key.y = 0.0f;
				key.z = static_cast<aqua::Float32>(rKey[2]);
				if (fabs(key.z) < epsilon)
					key.z = 0.0f;
				key.w = static_cast<aqua::Float32>(rKey[3]);
				if (fabs(key.w) < epsilon)
					key.w = 0.0f;
				key.t = static_cast<aqua::Float32>(rtrack->time(i));				
				
			}
			break;
			case eTranslationKey:
			{
				const aqua::Float32 epsilon = 0.001f;				
				AnimationTranslationTrack* ttrack = trackPointer_.ttrack_;
				FbxVector4 tKey( ttrack->key(i) );
				key.x = static_cast<aqua::Float32>(tKey[0]);
				if (fabs(key.x) < epsilon)
					key.x = 0.0f;				
				key.y = static_cast<aqua::Float32>(tKey[1]);
				if (fabs(key.y) < epsilon)
					key.y = 0.0f;				
				key.z = static_cast<aqua::Float32>(tKey[2]);
				if (fabs(key.z) < epsilon)
					key.z = 0.0f;				
				key.w = 0.0f;
				key.t = static_cast<aqua::Float32>(ttrack->time(i));
				
			}
			break;
		}
		return;
	}
	
	void sort()
	{
		switch (kind_)
		{
			case eTranslationKey:
				std::sort((*trackPointer_.ttrack_).begin(), (*trackPointer_.ttrack_).end());
				break;
			case eRotationKey:
				std::sort((*trackPointer_.rtrack_).begin(), (*trackPointer_.rtrack_).end());
				break;
			default:
				break;
		}
	}
	void destroy()
	{
		switch (kind_)
		{
			case eTranslationKey:
				delete trackPointer_.ttrack_;
				break;
			case eRotationKey:
				delete trackPointer_.rtrack_;
				break;
			default:
				break;
		}
		reset();
	}

	AnimationTrackPointer& operator=(const AnimationTrackPointer& other)
	{
		kind_ = other.kind_;
		switch (kind_)
		{
			case eTranslationKey:
				trackPointer_.ttrack_ = other.trackPointer_.ttrack_;
				break;
			case eRotationKey:
				trackPointer_.rtrack_ = other.trackPointer_.rtrack_;
				break;
			default:
				break;
		}
		return *this;
	}

	operator AnimationTranslationTrack*()
	{
		wxASSERT(kind_ == eTranslationKey);
		return trackPointer_.ttrack_;
	}

	operator AnimationRotationTrack*()
	{
		wxASSERT(kind_ == eRotationKey);
		return trackPointer_.rtrack_;
	}

	operator AnimationTranslationTrack*() const
	{
		wxASSERT(kind_ == eTranslationKey);
		return trackPointer_.ttrack_;
	}

	operator AnimationRotationTrack*() const
	{
		wxASSERT(kind_ == eRotationKey);
		return trackPointer_.rtrack_;
	}
		
	operator bool()
	{
		bool result = false;
		switch (kind_)
		{
			case eTranslationKey:
				result = (trackPointer_.ttrack_ != 0);
				break;
			case eRotationKey:
				result = (trackPointer_.rtrack_ != 0);
				break;
			default:					
				break;				   
		}
		return result;
	}
		
};

/** Creates new animation tracks. */
class AnimationTrackFactory
{
public:
	static AnimationTrackPointer newTrack(eAnimationKeyType kind)
	{
		AnimationTrackPointer result;
		result.kind_ = kind;
		switch (kind)
		{
			case eTranslationKey:
				result.trackPointer_.ttrack_ = new AnimationTranslationTrack();
				break;
			case eRotationKey:
				result.trackPointer_.rtrack_ = new AnimationRotationTrack();
				break;
			default:
				//wxFAIL("Unknown track type");
				result.trackPointer_.data_ = 0;
				break;
		}
		return result;
	}
};

#endif
