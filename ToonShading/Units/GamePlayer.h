#pragma once
#include "GameUnit.h"

class GamePlayer : public GameUnit
{
private:
	enum class DebugMode;
	enum class LowerAction;
	enum class UpperAction;

public:		//public Function
	GamePlayer(wstring matFile, wstring meshFile, wstring animPath, ExecuteValues* values = NULL);
	~GamePlayer();

	void Update();
	void Render();
	void PreRender();
	void PreRender2();
	void ImGuiRender();

	class Bullet* GetBullet() { return bullet; }

private:
	void LoadAnimation(wstring path);

	void HandleInput();
	void HandleMove();
	void HandleRotation();
	void HandleFire();

	void PlayLowerAction(LowerAction action);
	void PlayLowerAction(LowerAction action, float startTime);
	void PlayUpperAction(UpperAction action);
	void PlayUpperAction(UpperAction action, float startTime);

	void ActionMovement(D3DXVECTOR3 direction);
	void ActionRotation();

private:	//private enum action, model
	enum class DebugMode
	{
		None = 0, NeverDie, Superman, God,
		//superman - 원샷 원킬, god = neverdie + superman
	};
	enum class LowerAction
	{
		//얘가 가질 액션들 전부
		Unknown = 0,
		Idle, Walk, Run,
		BackWalk, Damage,
		LeftTurn, RightTurn,
		ForwardDead, BackwardDead,
		LeftDead, RightDead,
		BoosterPrepare, BoosterActive, BoosterFinish, BoosterBreak,
		BoosterLeftTurn, BoosterRightTurn,
		Count,
	};
	enum class UpperAction
	{
		Unknown = 0,
		Idle, Run, Damage, WeaponChange,
		ForwardNonFire, LeftNonFire, RightNonFire,
		ForwardMachineGunFire, LeftMachineGunFire, RightMachineGunFire, ReloadMachineGun,
		ForwardShotgunFire, LeftShotgunFire, RightShotgunFire, ReloadShotgun,
		ForwardHandgunFire, LeftHandgunFire, RightHandgunFire, ReloadHandgun,
		ForwardDead, BackwardDead, LeftDead, RightDead,
		BoosterPrepare, BoosterActive, BoosterFinish, BoosterBreak,
		BoosterLeftTurn, BoosterRightTurn,
		Count,
	};

private:	//private variables
	ExecuteValues* values;

	DebugMode debugMode;

	LowerAction currentLowerAction;
	UpperAction currentUpperAction;

	LowerAction prepareLowerAction;
	UpperAction prepareUpperAction;

	AnimationPlayMode modeOnce;
	AnimationPlayMode modeRepeat;

	vector<UINT> indexLowerAnimations;	//추가하면 해당 추가된 번호 저장받는 놈
	vector<UINT> indexUpperAnimations;

	D3DXVECTOR3 moveDirection;

	struct GamePlayerSpec* specData;
	struct GamePlayerInput* input;

	class ModelBone* boneWaist;
	D3DXMATRIX boneWaistSrc;

	class Bullet* bullet;
	
private:	
	float runDurationTime;
	float walkDurationTime;
	float boosterFinishDurationTime;
	float boosterBreakDurationTime;
	float weaponChangeDurationTime;

	float actionElapsedTime;
	float moveElapsedTime;

	float rootRotationAngle;
	float rootElapsedAngle;
	float currentWaistAngle;

	float weaponTime;
	float weaponDelay;

	//동작 바뀔지 묻는 것
	bool bOverwriteLowerAction;
	bool bOverwriteUpperAction;

	bool bActiveBooster;
	bool bRun;
};