#pragma once
#include "GameModel.h"
#include "AnimationClip.h"

/*
�갡 ���� ������ �� ��
��� ������ GameRender���� ��ӹ޾ƿ��� ����(Model, World, Skybox)
�׸��� GameModel���� GameAnimation�� �������, �ű⼭ ��ӹ޴� GameUnit�� ���� ��
unit�� ���� ���� - ���� �ű⼭ player�� enemy�� ����, ���� grund�� ���� 4���� ���� ��
*/
struct ExecuteValues;

class GameAnimationModel : public GameModel
{
public:
	GameAnimationModel(wstring matFile, wstring meshFile, ExecuteValues* value = NULL);
	virtual ~GameAnimationModel();

	UINT AddClip(wstring file);
	UINT AddClip(struct AnimationClip* clip);
	void RemoveClip(UINT index);
	void ClearClips();

	struct AnimationClip* GetClip(UINT index);
	struct AnimationBlender* GetBlenderFromBoneName(wstring name);

	bool Play(UINT index, enum class AnimationPlayMode mode, bool negative = true);
	bool Play(UINT index, float startTime, float blendTime, float timeScaleFactor, enum class AnimationPlayMode mode, bool negative = true);

	float PlayTime(wstring boneName);

	virtual void Update();
	virtual void PreRender();
	virtual void ImGuiRender();

private:
	vector<struct AnimationBlender*> blenders;
	vector<struct AnimationClip*> clips;
};