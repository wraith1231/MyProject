#pragma once
#include "GameEnemy.h"

struct ExecuteValues;
class EnemyTank : public GameEnemy
{
private:
	enum class Action;

public:
	EnemyTank(wstring matFile, wstring meshFile, ExecuteValues* value = NULL);// , wstring animPath);
	~EnemyTank();

	void Update();
	void PreRender();

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
	bool bOverwriteAction;

	Action currentAction;
	Action engageAction;	//engage : 사로잡다, 관심을 끌다

	UINT indexFireWeaponBone;
	UINT indexTurretBone;

	D3DXMATRIX transformTurret;
	float turretAngleSpeed;

	//float rotate;

private:
	enum class Action
	{
		Unknown = 0,
		Idle, Damage, Move, 
		Fire, Reload, 
		LeftTurn, RightTurn,
		Dead,
		Count,
	};

};