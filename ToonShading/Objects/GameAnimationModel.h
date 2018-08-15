#pragma once
#include "GameModel.h"
#include "AnimationClip.h"

/*
얘가 실제 렌더링 할 놈
상속 구조가 GameRender에서 상속받아오는 형태(Model, World, Skybox)
그리고 GameModel에서 GameAnimation을 만들었음, 거기서 상속받는 GameUnit이 나올 것
unit이 전반 관리 - 이제 거기서 player랑 enemy로 갈림, 현재 grund는 적이 4종류 정도 됨
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