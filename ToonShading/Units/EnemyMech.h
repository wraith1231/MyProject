#pragma once
#include "GameEnemy.h"

//tank ���� �������� ��κ��� ���

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
	bool bMoveBlocked;		//�� �����̰� �ϴ� ��
	
	Action currentAction;
	Action engageAction;	//engage : ������, ������ ����

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