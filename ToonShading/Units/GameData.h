#pragma once
#include "stdafx.h"

enum class UnitClassId
{
	Unknown = 0,
	LightMech, HeavyMech, Tank, Boss,
	Count,
};

enum class UnitTypeId
{
	Unknown = 0,
	//Player
	Grund, Kiev, Mark, Yager,
	//Enemy
	Cameleer, Maoming, Duskmas, Hammer, Tiger, PhantomBoss,
	Count,
};

struct GameDataSpec
{
	wstring file;

	UnitClassId Class = UnitClassId::LightMech;
	UnitTypeId Type = UnitTypeId::Grund;

	UINT Life = 3500;
	
	float MechRadius = 2.0f;
	float TurnAngle = 45.0f;
	
	wstring MaterialFile = Models + L"Grund/Grund.material";
	wstring MeshFile = Models + L"Grund/Grund.mesh";
	wstring AnimationFolder = Models + L"Grund/";
	wstring DefaultWeaponFile = L"";
};

struct GamePlayerSpec : public GameDataSpec
{
	float RunSpeed = 7.0f;
	float WalkSpeed = 5.0f;
	float WalkBackwardSpeed = 4.0f;
	float CriticalDamageTime = 1.0f;

	float BoosterSpeed = 32.0f;
	float BoosterActiveTime = 1.5f;
	float BoosterPrepareTime = 0.5f;
	float BoosterCoolTime = 8.0f;
	float BoosterTurnAngle = 70.0f;

	D3DXVECTOR3 CameraTargetOffset = D3DXVECTOR3(0, 2, 0);
	D3DXVECTOR3 CameraPositionOffset = D3DXVECTOR3(0, 0, -5); 
};

struct GameEnemySpec : public GameDataSpec
{
	float MoveSpeed = 4.0f;

};