#pragma once
#include "AnimationKeyframe.h"
#include "../Utilities/BinaryFile.h"

struct AnimationClip
{
	float Duration;
	vector<AnimationKeyframe *> Keyframes;

	void ReadData(wstring file)
	{
		BinaryReader* r = new BinaryReader();
		r->Open(file);

		UINT keyframeCount = r->UInt();
		Duration = r->Float();

		for (UINT i = 0; i < keyframeCount; i++)
		{
			AnimationKeyframe* keyframe = new AnimationKeyframe();

			keyframe->BoneName = String::ToWString(r->String());
			keyframe->FrameCount = r->UInt();
			keyframe->Duration = r->Float();
			keyframe->Interval = r->Float();


			UINT size = 0;

			size = r->UInt();
			if (size > 0)
			{
				keyframe->Translations.assign(size, D3DXVECTOR3());

				void* ptr = (void *)&keyframe->Translations[0];
				r->Byte(&ptr, sizeof(D3DXVECTOR3) * size);
			}

			size = r->UInt();
			if (size > 0)
			{
				keyframe->Rotations.assign(size, D3DXQUATERNION());

				void* ptr = (void *)&keyframe->Rotations[0];
				r->Byte(&ptr, sizeof(D3DXQUATERNION) * size);
			}

			size = r->UInt();
			if (size > 0)
			{
				keyframe->Scales.assign(size, D3DXVECTOR3());

				void* ptr = (void *)&keyframe->Scales[0];
				r->Byte(&ptr, sizeof(D3DXVECTOR3) * size);
			}

			size = r->UInt();
			if (size > 0)
			{
				keyframe->Times.assign(size, float());

				void* ptr = (void *)&keyframe->Times[0];
				r->Byte(&ptr, sizeof(float) * size);
			}

			Keyframes.push_back(keyframe);
		}

		r->Close();
		SAFE_DELETE(r);
	}
};