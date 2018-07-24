#pragma once
#include "stdafx.h"

enum class AnimationPlayMode
{
	Once = 0, Repeat,
};

struct AnimationKeyframeData
{
	bool bHasTranslation = false;
	bool bHasRotation = false;
	bool bHasScale = false;

	D3DXVECTOR3 Translation;
	D3DXQUATERNION Rotation;
	D3DXVECTOR3 Scale;
};

struct AnimationKeyframe
{
	wstring BoneName = L"";

	UINT FrameCount = 0;

	float Interval = 0.0f;
	float Duration = 0.0f;

	vector<D3DXVECTOR3> Translations;
	vector<D3DXQUATERNION> Rotations;
	vector<D3DXVECTOR3> Scales;
	vector<float> Times;

	bool HasTranslation() { return Translations.size() > 0; }
	bool HasRotation() { return Rotations.size() > 0; }
	bool HasScale() { return Scales.size() > 0; }
	bool HasTime() { return Times.size() > 0; }

	bool FixedTranslation() { return Translations.size() == 1; }
	bool FixedRotation() { return Rotations.size() == 1; }
	bool FixedScale() { return Scales.size() == 1; }

	/*D3DXMATRIX GetMatrix(UINT index)
	{
	D3DXMATRIX temp;
	D3DXMatrixIdentity(&temp);

	if (HasRotation() == true)
	{
	if (FixedRotation() == true)
	D3DXMatrixRotationQuaternion(&temp, &Rotations[0]);
	else
	D3DXMatrixRotationQuaternion(&temp, &Rotations[index]);
	}

	if (HasTranslation() == true)
	{
	D3DXVECTOR3 translation = Translations[0];

	if (FixedTranslation() == false)
	translation = Translations[index];

	temp._41 = translation.x;
	temp._42 = translation.y;
	temp._43 = translation.z;
	}

	if (HasScale() == true)
	{
	D3DXVECTOR3 scale = Scales[0];

	if (FixedScale() == false)
	scale = Scales[index];

	D3DXMATRIX matrix;
	D3DXMatrixScaling(&matrix, scale.x, scale.y, scale.z);

	temp = temp * matrix;
	}

	return temp;
	}*/

private:
	UINT GetKeyframeIndex(float time)
	{
		int start, end, middle;

		start = 0;
		end = FrameCount - 1;

		if (time >= Times[end])
			return end;


		do
		{
			middle = (start + end) / 2;

			if (end - start <= 1)
			{
				break;
			}
			else if (Times[middle] < time)
			{
				start = middle;
			}
			else if (Times[middle] > time)
			{
				end = middle;
			}
			else
			{
				start = middle;
				break;
			}
		} while ((end - start) > 1);

		return start;
	}

	void CalcKeyframeIndex(float time, AnimationPlayMode mode, OUT UINT& index1, OUT UINT& index2, OUT float& interpolatedTime)
	{
		index1 = index2 = 0;
		interpolatedTime = 0.0f;

		if (HasTime() == true)
			index1 = GetKeyframeIndex(time);
		else
			index1 = (UINT)(time / Interval);

		if (mode == AnimationPlayMode::Once)
			index2 = (index1 >= FrameCount - 1) ? index1 : index1 + 1;
		else if (mode == AnimationPlayMode::Repeat)
			index2 = (index1 >= FrameCount - 1) ? 0 : index1 + 1;

		if (index1 >= FrameCount - 1)
		{
			index1 = index2 = FrameCount - 1;

			interpolatedTime = 1.0f;
		}
		else
		{
			if (HasTime() == true)
				interpolatedTime = (time - Times[index1]) / (Times[index2] - Times[index1]);
			else
				interpolatedTime = Math::Modulo(time, Interval) / Interval;
		}
	}


	D3DXMATRIX GetInterpolateMatrix(UINT index1, UINT index2, float t)
	{
		D3DXMATRIX temp;
		D3DXMatrixIdentity(&temp);

		if (HasRotation() == true)
		{
			D3DXQUATERNION q;

			if (FixedRotation() == true)
				q = Rotations[0];
			else
				D3DXQuaternionSlerp(&q, &Rotations[index1], &Rotations[index2], t);

			D3DXMatrixRotationQuaternion(&temp, &q);
		}

		if (HasTranslation() == true)
		{
			D3DXVECTOR3 translation = Translations[0];

			if (FixedTranslation() == false)
				D3DXVec3Lerp(&translation, &Translations[index1], &Translations[index2], t);

			temp._41 = translation.x;
			temp._42 = translation.y;
			temp._43 = translation.z;
		}

		if (HasScale() == true)
		{
			D3DXVECTOR3 scale = Scales[0];

			if (FixedScale() == false)
				D3DXVec3Lerp(&scale, &Scales[index1], &Scales[index2], t);

			D3DXMATRIX matrix;
			D3DXMatrixScaling(&matrix, scale.x, scale.y, scale.z);

			temp = temp * matrix;
		}

		return temp;
	}

public:
	D3DXMATRIX GetInterpolateMatrix(float time, AnimationPlayMode mode)
	{
		UINT index1 = 0;
		UINT index2 = 0;
		float interpolatedTime = 0.0f;

		CalcKeyframeIndex(time, mode, index1, index2, interpolatedTime);

		return GetInterpolateMatrix(index1, index2, interpolatedTime);
	}

private:
	AnimationKeyframeData GetInterpolateKeyframe(UINT index1, UINT index2, float t)
	{
		AnimationKeyframeData keyframe;


		if (HasRotation() == true)
		{
			if (FixedRotation() == true)
				keyframe.Rotation = Rotations[0];
			else
				D3DXQuaternionSlerp(&keyframe.Rotation, &Rotations[index1], &Rotations[index2], t);
		}
		else
		{
			D3DXQuaternionIdentity(&keyframe.Rotation);
		}
		keyframe.bHasRotation = HasRotation();


		if (HasTranslation() == true)
		{
			if (FixedTranslation() == true)
				keyframe.Translation = Translations[0];
			else
				D3DXVec3Lerp(&keyframe.Translation, &Translations[index1], &Translations[index2], t);
		}
		else
		{
			keyframe.Translation = D3DXVECTOR3(0, 0, 0);
		}
		keyframe.bHasTranslation = HasTranslation();


		if (HasScale() == true)
		{
			if (FixedScale() == true)
				keyframe.Scale = Scales[0];
			else
				D3DXVec3Lerp(&keyframe.Scale, &Scales[index1], &Scales[index2], t);
		}
		else
		{
			keyframe.Scale = D3DXVECTOR3(1, 1, 1);
		}
		keyframe.bHasScale = HasScale();

		return keyframe;
	}
	//ctrl + -, ctrl + shift + - 이전 페이지 돌아가기
	//ctrl + k ~ ctrl + k = 책갈피, ctrl + k ~ ctrl + p = 이전 책갈피로 이동
	//아래 region은 c# 기능
#pragma region 후비적

#pragma endregion
	//visual studio 마켓에서 most popular에 뒤져보면 좋은거 많음
	//c에선 visual assist가 끝판왕이라고
public:
	AnimationKeyframeData GetInterpolateKeyframe(float time, AnimationPlayMode mode)
	{
		UINT index1 = 0;
		UINT index2 = 0;
		float interpolateTime = 0.0f;

		CalcKeyframeIndex(time, mode, index1, index2, interpolateTime);

		return GetInterpolateKeyframe(index1, index2, interpolateTime);
	}
};