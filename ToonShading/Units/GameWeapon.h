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

public:
	enum class WeaponState
	{
		Ready = 0, Firing, Reloading, Count,
	};

private:
	vector<class Model*> models;

	struct GameWeaponSpec* specData;
	WeaponState state;

	vector<UINT> indexWeaponFireDummy; //무기 붙일 곳 (총구)
	vector<UINT> indexWeaponAttachDummy; //무기 붙일 곳

};