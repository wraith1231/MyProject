#include "stdafx.h"
#include "GameAnimationModel.h"

#include "AnimationBlender.h"

GameAnimationModel::GameAnimationModel(wstring matFile, wstring meshFile)
	: GameModel(matFile, meshFile)
{
	for (UINT i = 0; i < model->BoneCount(); i++)
	{
		ModelBone* bone = model->Bone(i);

		AnimationBlender* blender = new AnimationBlender();
		blender->Name = bone->Name();

		blenders.push_back(blender);
	} // ��� pair�� �ٲ�� ���� �̴�� ������ �� ������
}

GameAnimationModel::~GameAnimationModel()
{
	for (AnimationBlender* blender : blenders)
		SAFE_DELETE(blender);
}

UINT GameAnimationModel::AddClip(wstring file)
{
	AnimationClip* clip = new AnimationClip();
	clip->ReadData(file);

	return AddClip(clip);
}

UINT GameAnimationModel::AddClip(AnimationClip * clip)
{
	clips.push_back(clip);

	return clips.size() - 1;
}

void GameAnimationModel::RemoveClip(UINT index)
{
	SAFE_DELETE(clips[index]);
	clips.erase(clips.begin() + index);
}

void GameAnimationModel::ClearClips()
{
	for (AnimationClip* clip : clips)
		SAFE_DELETE(clip);

	clips.clear();
}

AnimationClip * GameAnimationModel::GetClip(UINT index)
{
	return clips[index];
}

AnimationBlender * GameAnimationModel::GetBlenderFromBoneName(wstring name)
{
	for (AnimationBlender* blender : blenders)
	{
		if (blender->Name == name)
			return blender;
	}

	return NULL;
}

bool GameAnimationModel::Play(UINT index, AnimationPlayMode mode)
{
	//blend time�� 0�̸� �ȼ��´� ���
	return Play(index, 0.0f, 0.0f, 1.0f, mode);
}

bool GameAnimationModel::Play(UINT index, float startTime, float blendTime, float timeScaleFactor, AnimationPlayMode mode)
{
	AnimationClip* clip = GetClip(index);
	if (clip == NULL) return false;

	for (AnimationKeyframe* keyframe : clip->Keyframes)
	{
		AnimationBlender* blender = GetBlenderFromBoneName(keyframe->BoneName);

		blender->AddKeyframe(keyframe, startTime, blendTime, timeScaleFactor, mode);
	}

	return true;
}

float GameAnimationModel::PlayTime(wstring boneName)
{
	for (AnimationBlender* blender : blenders)
	{
		if (blender->Name == boneName)
			return blender->Current->PlayTime;
	}

	return 0.0f;
}

void GameAnimationModel::Update()
{
	CalcPosition();

	//clip�� ������ play �Ƚ�Ŵ
	if (clips.size() > 0)
	{
		for (UINT i = 0; i < model->BoneCount(); i++)
		{
			//blender�� �ش� bone�� ������ ���� �� ����
			//��� null�� �ƴ�
			AnimationBlender* blender = blenders[i];
			if (blender->Exist() == true)
			{
				float time = Time::Delta();

				D3DXMATRIX mat = blender->GetKeyframeMatrix(time);

				model->Bone(i)->Transform(mat);
			}
		}//for(i<model->bonecount)
	}

	D3DXMATRIX transformed = Transformed();

	model->CopyAbsoluteBoneTo(transformed, boneTransforms);
}

void GameAnimationModel::Render()
{
	//�ٲ��ٰ� �θ��� boneTransforms�� ���ָ� ��
	GameModel::Render();
}

void GameAnimationModel::PreRender()
{
	GameModel::PreRender();
}

void GameAnimationModel::PreRender2()
{
	GameModel::PreRender2();
}

void GameAnimationModel::ImGuiRender()
{
}
