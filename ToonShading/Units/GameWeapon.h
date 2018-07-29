#pragma once
#include "../Objects/GameRender.h"

class GameWeapon : public GameRender
{
public:
	enum class WeaponState;

public:
	GameWeapon();
	~GameWeapon();

	void Update();
	void Render();
	void PreRender();
	void PreRender2();
	void ImGuiRender();

	struct GameWeaponSpec* SpecData() { return specData; }
	void SpecData(struct GameWeaponSpec* spec) { specData = spec; }

	WeaponState State() { return state; }
	enum class WeaponType WeaponType();

	float ReloadTime() { return reloadTime; }
	void ReloadTime(float& val) { reloadTime = val; }

public:
	enum class WeaponState
	{
		Ready = 0, Firing, Reloading, Count,
	};

private:
	vector<class Model*> models;

	struct GameWeaponSpec* specData;
	float reloadTime;
	WeaponState state;

	vector<UINT> indexWeaponFireDummy; //무기 붙일 곳 (총구)
	vector<UINT> indexWeaponAttachDummy; //무기 붙일 곳

};