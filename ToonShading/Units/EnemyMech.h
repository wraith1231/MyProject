#pragma once
#include "GameEnemy.h"

//tank 말고 나머지는 얘로부터 상속

class EnemyMech : public GameEnemy
{
private:
	enum class Action;

public:
	EnemyMech(wstring matFile, wstring meshFile);// , wstring animPath);
	~EnemyMech();

	void Update();
	void Render();

	void PlayAction(Action action) { currentAction = action; }

	void ActionIdle();
	bool ActionFire();
	void ActionHit(GameUnit* attacker);
	bool ActionReload(class GameWeapon* weapon);
	void ActionDamage(GameUnit* attacker);
	void ActionDead(D3DXVECTOR3 attackerPosition);

private:
	void ProcessAction();

	void OnAiSearch(AiState* state);
	void OnAiMove(AiState* state);
	void OnAiAttack(AiState* state);
	void OnAiTurnLeft(AiState* state);
	void OnAiTurnRight(AiState* state);

private:
	float defaultAnimSpeed;
	float fireDelayElapsedTime;
	float moveDurationTime;
	float moveElapsedTime;

	bool bCriticalDamage;
	bool bMoveBlocked;		//못 움직이게 하는 것
	
	Action currentAction;
	Action engageAction;	//engage : 사로잡다, 관심을 끌다

	vector<UINT> indexAnimation;

	UINT indexLeftFireWeaponBone;
	UINT indexRightFireWeaponBone;

private:
	enum class Action
	{
		Unknown = 0,
		Idle, Damage, 
		Fire, Melee, Reload,
		ForwardWalk, BackwardWalk, LeftWalk, RightWalk,
		LeftTurn, RightTurn,
		ForwardDead, BackwardDead, LeftDead, RightDead,
		Count,
	};

};