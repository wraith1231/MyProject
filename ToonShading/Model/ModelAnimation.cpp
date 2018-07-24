#include "stdafx.h"
#include "ModelAnimation.h"

#include "Model.h"

ModelAnimation::ModelAnimation(bool loop, wstring name, vector<float> times, vector<D3DXVECTOR3> scale, vector<D3DXVECTOR3> rotate, vector<D3DXVECTOR3> trans)
{
	this->name = name;
	this->loop = loop;

	this->time.assign(times.begin(), times.end());
	this->scale.assign(scale.begin(), scale.end());
	this->rotate.assign(rotate.begin(), rotate.end());
	this->trans.assign(trans.begin(), trans.end());

	index = 0;
	frameTime = 0;
	play = true;

	D3DXMatrixIdentity(&transform);
}

ModelAnimation::~ModelAnimation()
{
}

void ModelAnimation::Update()
{
	if (time.size() > 0 && play == true)
	{
		float delta = Time::Get()->Delta();
		frameTime += delta;

		//deltaa = delta;

		UINT indexNext = index + 1;
		if (indexNext >= time.size())
			indexNext = indexNext - time.size();

		float del1, del2;
		del1 = frameTime - time[index];
		if (indexNext > index)
			del2 = time[indexNext] - time[index];
		else
			del2 = time[index] - time[index-1];

		float tDelta = del1 / del2;
		//tDeltaa = tDelta;
		D3DXVECTOR3 s, t;
		D3DXVec3Lerp(&s, &scale[index], &scale[indexNext], tDelta);
		D3DXVec3Lerp(&t, &trans[index], &trans[indexNext], tDelta);

		if (frameTime >= 16.2)
			int a = 10;
		D3DXMATRIX S, T;
		D3DXMatrixScaling(&S, s.x, s.y, s.z);
		D3DXMatrixTranslation(&T, t.x, t.y, t.z);

		D3DXQUATERNION q;
		D3DXQUATERNION q1, q2;
		D3DXVECTOR3 r1, r2;
		r1 = rotate[index];
		r2 = rotate[indexNext];

		r1.x = Math::ToRadian(r1.x);
		r1.y = Math::ToRadian(r1.y);
		r1.z = Math::ToRadian(r1.z);

		r2.x = Math::ToRadian(r2.x);
		r2.y = Math::ToRadian(r2.y);
		r2.z = Math::ToRadian(r2.z);

		D3DXQuaternionRotationYawPitchRoll(&q1, r1.y, r1.x, r1.z);
		D3DXQuaternionRotationYawPitchRoll(&q2, r2.y, r2.x, r2.z);

		D3DXQuaternionSlerp(&q, &q1, &q2, tDelta);

		D3DXMATRIX R;
		D3DXMatrixRotationQuaternion(&R, &q);

		if (indexNext > index)
		{
			if (frameTime >= time[indexNext])
			{
				index++;
				if (index >= time.size())
				{
					index = 0;
					frameTime = 0;

					if (loop == false)
					{
						play = false;
					}
				}
			}
		}
		else
		{
			if (frameTime >= time[index] + (time[index] - time[index - 1]))
			{
				index++;
				if (index >= time.size())
				{
					index = 0;
					frameTime = 0;

					if (loop == false)
					{
						play = false;
					}
				}

			}
		}
		transform = S * R * T;
	}
}

D3DXMATRIX ModelAnimation::GetTransform()
{
	return transform;
}
