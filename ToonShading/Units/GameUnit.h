#pragma once
#include "../Objects/GameAnimationModel.h"

struct ExecuteValues;
class GameWeapon;
class GameUnit : public GameAnimationModel
{
public:
	GameUnit(wstring matFile, wstring meshFile, ExecuteValues* value = NULL);
	virtual ~GameUnit();

	virtual void Update();

	virtual void NormalRender();
	virtual void DepthRender();
	virtual void DiffuseRender();

	virtual void ImGuiRender();

	void Life(float val) { life = val; }
	float Life() { return life; }

	void MaxLife(float val) { maxLife = val; }
	float MaxLife() { return maxLife; }

	void SpawnPoint(D3DXMATRIX& mat) { spawnPoint = mat; }
	D3DXMATRIX SpawnPoint() { return spawnPoint; }

	bool IsFullLife() { return life == maxLife; }
	bool IsDead() { return life < 1.0f; }

	GameWeapon* DefaultWeapon() { return weapons[0]; }
	GameWeapon* CurrentWeapon() { return currentWeapon; }

	void CreateWeapon(wstring matFile, wstring meshFile);
	void SelectWeapon(UINT slot);
	GameWeapon* Weapon(UINT slot) { return weapons[slot]; }

	virtual void ActionIdle() {}
	virtual bool ActionFire() { return false; }
	virtual bool ActionMelee() { return false; }
	virtual void ActionHit(GameUnit* attacker) {}
	virtual bool ActionReload(GameWeapon* weapon) { return false; }
	virtual void ActionDamage(GameUnit* attacker) { }
	virtual void ActionDead(D3DXVECTOR3 attackerPosition) {}

protected:
	D3DXMATRIX spawnPoint;

	GameWeapon* currentWeapon;
	int currentWeaponSlot;
	vector<GameWeapon*> weapons;

	float life;
	float maxLife;

};