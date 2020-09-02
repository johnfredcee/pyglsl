#ifndef ANIMATIONROTATIONKEY_H
#define ANIMATIONROTATIONKEY_H

struct AnimationRotationKey
{
	typedef FbxQuaternion KeyType;

	AnimationRotationKey(FbxQuaternion quat) : key_(quat)
	{

	};

	KeyType& key()
	{
		return key_;
	}


	const KeyType& key() const
	{
		return key_;
	}

	const enum eAnimationKeyType keyType() const
	{
		return eRotationKey;
	}

private:
	KeyType key_;
};



#endif
