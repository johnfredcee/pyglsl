#ifndef ANIMATIONPOSITIONKEY_H
#define ANIMATIONPOSITIONKEY_H

struct AnimationTranslationKey 
{
	typedef KFbxVector4 KeyType;

	AnimationTranslationKey(KFbxVector4 vec) :	key_(vec)
	{

	};

	KFbxVector4& key()
	{
		return key_;
	}

	const KFbxVector4& key() const
	{
		return key_;
	}

	const enum eAnimationKeyType keyType() const
	{
		return eTranslationKey;
	}

private:
	KFbxVector4 key_;

};

#endif
