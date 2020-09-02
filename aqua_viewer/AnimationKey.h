#ifndef ANIMATIONKEY_H
#define ANIMATIONKEY_H
	
template <class T, class K> class AnimationTrackKey 
{
public:
	AnimationTimeStamp<T> time_;
	K key_;

	typedef typename K::KeyType KeyType;
	typedef typename T::TimeType TimeType;

	AnimationTrackKey(const AnimationTimeStamp<typename T>& time, const KeyType& key) : time_(time), key_(key)
	{
			
	}
		
	const double time() const
	{
		return static_cast<const double>(time_);
	}

	const bool operator==(const AnimationTrackKey& other) const
	{
		return time_ == other.time_;
	}

	const bool operator<(const AnimationTrackKey& other) const
	{
		return time_ < other.time_;
	}

	KeyType& key()
	{
		return key_.key();
	}

	const KeyType& key() const
	{
		return key_.key();
	}
};

#endif
