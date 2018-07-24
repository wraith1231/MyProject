#pragma once
#include "AnimationKeyframe.h"

struct AnimationBinder
{
	AnimationPlayMode PlayMode = AnimationPlayMode::Once;

	float Speed = 0.0f;
	float PlayTime = 0.0f;

	AnimationKeyframe* Keyframe;

	float Duration() { return Keyframe->Duration; }
	bool PlayDone() { return PlayTime >= Duration(); }

	void BindKeyframe(AnimationKeyframe* keyframe, float start, float speed, AnimationPlayMode mode)
	{
		Keyframe = keyframe;

		PlayTime = start;
		Speed = speed;
		PlayMode = mode;
	}

	void Initialize()
	{
		Keyframe = NULL;

		PlayTime = 0.0f;
		Speed = 1.0f;
		PlayMode = AnimationPlayMode::Repeat;
	}

	void CopyTo(AnimationBinder* clone)
	{
		clone->Keyframe = this->Keyframe;
		clone->PlayTime = this->PlayTime;
		clone->Speed = this->Speed;
		clone->PlayMode = this->PlayMode;
	}

	AnimationKeyframeData GetKeyframeData(float time)
	{
		assert(Keyframe != NULL);

		if (Speed == 0.0f)
			return AnimationKeyframeData();


		PlayTime += Speed * time;
		if (PlayMode == AnimationPlayMode::Once)
		{
			if (PlayTime >= Duration())
				PlayTime = Keyframe->Duration;
		}
		else if (PlayMode == AnimationPlayMode::Repeat)
		{
			if (PlayTime >= Duration())
				PlayTime = Math::Modulo(PlayTime, Duration());
		}

		return Keyframe->GetInterpolateKeyframe(PlayTime, PlayMode);
	}

	D3DXMATRIX GetKeyframeMatrix(float time)
	{
		assert(Keyframe != NULL);

		if (Speed == 0.0f)
			return D3DXMATRIX();


		PlayTime += Speed * time;
		if (PlayMode == AnimationPlayMode::Once)
		{
			if (PlayTime >= Duration())
				PlayTime = Keyframe->Duration;
		}
		else if (PlayMode == AnimationPlayMode::Repeat)
		{
			if (PlayTime >= Duration())
				PlayTime = Math::Modulo(PlayTime, Duration());
		}

		return Keyframe->GetInterpolateMatrix(PlayTime, PlayMode);
	}
};