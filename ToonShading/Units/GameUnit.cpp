#include "stdafx.h"
#include "GameUnit.h"

#include "GameWeapon.h"
#include "GameData.h"

GameUnit::GameUnit(wstring matFile, wstring meshFile, ExecuteValues* value)
	: GameAnimationModel(matFile, meshFile, value)
	, currentWeapon(NULL)
{
	life = 0;
	maxLife = 0;

	D3DXMatrixIdentity(&spawnPoint);

	currentWeaponSlot = -1;
}

GameUnit::~GameUnit()
{

}

void GameUnit::Update()
{
	GameAnimationModel::Update();
}

void GameUnit::PreRender()
{
	GameAnimationModel::PreRender();
}

void GameUnit::ImGuiRender()
{
}

void GameUnit::CreateWeapon(wstring matFile, wstring meshFile)
{
	GameWeaponSpec* spec = new GameWeaponSpec();
	//TODO : Spec Data Loading

	GameWeapon* weapon = new GameWeapon();
	weapon->SpecData(spec);
	
	weapons.push_back(weapon);
	SelectWeapon(0);
}

void GameUnit::SelectWeapon(UINT slot)
{
	if (slot >= weapons.size()) return;
	currentWeapon = weapons[slot];
	currentWeaponSlot = slot;

	for (UINT i = 0; i < weapons.size(); i++)
	{
		if (i == slot)
		{
			weapons[i]->Visible(true);
			weapons[i]->Enable(true);
		}
		else
		{
			weapons[i]->Visible(false);
			weapons[i]->Enable(false);
		}
	}
}

