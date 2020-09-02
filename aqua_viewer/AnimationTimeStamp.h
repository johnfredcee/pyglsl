#ifndef ANIMATIONTIMESTAMP_H
#define ANIMATIONTIMESTAMP_H

enum eAnimationKeyType
{
	eDuffKey,
	eRotationKey,
	eTranslationKey,
	eEventKey
};
	
template <typename T> struct AnimationTimeStamp
{
	typedef T TimeType;

	AnimationTimeStamp() : time_(-1.0)
	{

	};

	AnimationTimeStamp(typename T time) : time_(time)
	{

	};

	const T time() const
	{
		return time_;
	}

	operator double() const
	{
		return static_cast<double>(time_);
	}
			
	const bool operator==(const AnimationTimeStamp& other) const
	{
		return time_ == other.time_;
	}

	const bool operator<(const AnimationTimeStamp& other) const
	{
		return time_ < other.time_;
	}


private:
	T time_;
};

#endif
