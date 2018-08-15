#include "stdafx.h"
#include "GameWeapon.h"

#include "GameData.h"

GameWeapon::GameWeapon()
	: GameRender()
	, specData(NULL)
{
	specData = new GameWeaponSpec();

	state = WeaponState::Ready;
	reloadTime = 0.0f;
}

GameWeapon::~GameWeapon()
{
	SAFE_DELETE(specData);
}

void GameWeapon::Update()
{

}

void GameWeapon::PreRender()
{
}

void GameWeapon::ImGuiRender()
{
}

WeaponType GameWeapon::WeaponType()
{ 
	return specData->Type; 
}
