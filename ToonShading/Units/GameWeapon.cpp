#include "stdafx.h"
#include "GameWeapon.h"

#include "GameData.h"

GameWeapon::GameWeapon()
	: GameRender()
	, specData(NULL)
{
	specData = new GameWeaponSpec();

	state = WeaponState::Ready;
}

GameWeapon::~GameWeapon()
{
	SAFE_DELETE(specData);
}

void GameWeapon::Update()
{
}

void GameWeapon::Render()
{
}

void GameWeapon::PreRender()
{
}

void GameWeapon::PreRender2()
{
}

void GameWeapon::ImGuiRender()
{
}

WeaponType GameWeapon::WeaponType()
{ 
	return specData->Type; 
}
