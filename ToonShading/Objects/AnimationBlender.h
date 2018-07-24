#pragma once
#include "AnimationBinder.h"

struct AnimationBlender
{
	AnimationBinder* Current = NULL;
	AnimationBinder* Next = NULL;

	wstring Name = L"";

	UINT BindCount = 0;
	float BlendTime = 0.0f;
	float ElapsedTime = 0.0f;

	bool Empty() { return BindCount == 0; }
	bool Exist() { return ((Empty() == false) && (Current != NULL)); }

	AnimationBlender()
	{
		Current = new AnimationBinder();
		Next = new AnimationBinder();

		BindCount = 0;
	}

	void ResetBinder()
	{
		Current->Initialize();
		Next->Initialize();

		BindCount = 0;
	}

	void ShiftBinder()
	{
		Current->Initialize();
		Next->CopyTo(Current);
		Next->Initialize();

		BindCount--;
	}

	void AddKeyframe(AnimationKeyframe* keyframe, float startTime, float blendTime, float timeScaleFactor, AnimationPlayMode mode)
	{
		BlendTime = blendTime;
		ElapsedTime = startTime;

		if (BlendTime == 0.0f)
		{
			ResetBinder();

			Current->BindKeyframe(keyframe, startTime, timeScaleFactor, mode);
		}
		else
		{
			//이게 들어올때 0, 1, 2, 3... 로 오면 동작들을 누적된 평균값으로 움직이게 함
			//FbxAnimLayer라는게 있는데 unity나 엔진들도 가지고 있음, 애니메이션을 layer로 갖는데
			//지금 구조는 layer를 두개만 쓴 셈, 0번째는 걷는거, 1번째는 뛰는걸로 했을때
			//걷는게 0~1.0, 뛰는게 0.8~2.0일때 사이 0.8~1.0을 섞어서 동작이 자연스레 전환되도록
			//하는 방식이 있음, 또 0번에 걷는거, 1번에 하체는 냅두고 손만 흔드는걸로 할때
			//걸으면서 손흔드는게 나옴 - 거기에 Curve기법도 있는데(당연히 다른 엔진에서도 용어만
			//다르고 쓰임 unity는 마스크?) spline이란 수식을 통해 3차방정식 형태의 커브구간으로
			// /\/방식으로 보간을 취해주는 기법 - 한 동작이 다음 동작으로 전환될 때 얼마나 부드럽게
			//전환될지 정하는게 커브값 
			if (BindCount == 2)
				ShiftBinder();

			if (BindCount == 0)
				Current->BindKeyframe(keyframe, startTime, timeScaleFactor, mode);
			else if (BindCount == 1)
				Next->BindKeyframe(keyframe, startTime, timeScaleFactor, mode);
		}

		BindCount++;
	}

	D3DXMATRIX Negative()
	{
		D3DXMATRIX scaleX;
		D3DXMatrixScaling(&scaleX, -1, 1, 1);

		D3DXMATRIX rotationX;
		D3DXMatrixRotationX(&rotationX, -Math::ToRadian(90));

		D3DXMATRIX rotationY;
		D3DXMatrixRotationY(&rotationY, Math::ToRadian(180));

		return scaleX * rotationX * rotationY;
	}

	D3DXMATRIX GetKeyframeMatrix(float time)
	{
		D3DXMATRIX matrix;

		ElapsedTime += time;
		if (BindCount <= 1)
		{
			matrix = Current->GetKeyframeMatrix(time);
		}
		else
		{
			float t = ElapsedTime / BlendTime;	//BlendTime : 다음 동작으로 넘어가는 시간
			if (t > 1.0f)
			{
				matrix = Next->GetKeyframeMatrix(time);

				ShiftBinder();
			}
			else
			{
				AnimationKeyframeData target;

				AnimationKeyframeData src[2];
				src[0] = Current->GetKeyframeData(time);
				src[1] = Next->GetKeyframeData(time);

				CalcBlendingKeyframe(&target, src, t);

				matrix = CreateTransformFromTarget(target);

			}
		}

		return Negative() * matrix * Negative();
	}

	void CalcBlendingKeyframe(AnimationKeyframeData* target, AnimationKeyframeData* src, float t)
	{
		//Translation
		if (src[0].bHasTranslation == true && src[1].bHasTranslation == true)
		{
			D3DXVec3Lerp(&target->Translation, &src[0].Translation, &src[1].Translation, t);
			target->bHasTranslation = true;
		}
		else if (src[0].bHasTranslation == true)
		{
			target->Translation = src[0].Translation;
			target->bHasTranslation = true;
		}
		else if (src[1].bHasTranslation == true)
		{
			target->Translation = src[1].Translation;
			target->bHasTranslation = true;
		}


		//Scale
		if (src[0].bHasScale == true && src[1].bHasScale == true)
		{
			D3DXVec3Lerp(&target->Scale, &src[0].Scale, &src[1].Scale, t);
			target->bHasScale = true;
		}
		else if (src[0].bHasScale == true)
		{
			target->Scale = src[0].Scale;
			target->bHasScale = true;
		}
		else if (src[1].bHasScale == true)
		{
			target->Scale = src[1].Scale;
			target->bHasScale = true;
		}


		//Rotation
		if (src[0].bHasRotation == true && src[1].bHasRotation == true)
		{
			D3DXQuaternionSlerp(&target->Rotation, &src[0].Rotation, &src[1].Rotation, t);
			target->bHasRotation = true;
		}
		else if (src[0].bHasRotation == true)
		{
			target->Rotation = src[0].Rotation;
			target->bHasRotation = true;
		}
		else if (src[1].bHasRotation == true)
		{
			target->Rotation = src[1].Rotation;
			target->bHasRotation = true;
		}
	}

	D3DXMATRIX CreateTransformFromTarget(AnimationKeyframeData target)
	{
		D3DXMATRIX matrix;

		if (target.bHasRotation == true)
			D3DXMatrixRotationQuaternion(&matrix, &target.Rotation);

		if (target.bHasTranslation == true)
		{
			matrix._41 = target.Translation.x;
			matrix._42 = target.Translation.y;
			matrix._43 = target.Translation.z;
		}

		if (target.bHasScale == true)
		{
			D3DXMATRIX temp;
			D3DXMatrixScaling(&temp, target.Scale.x, target.Scale.y, target.Scale.z);

			matrix = matrix * temp;
		}

		return matrix;
	}
};