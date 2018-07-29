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

enum class WeaponType
{
	Unknown = 0,
	CameleerGun, MaomingGun,
	DuskmasCannon, HammerCannon,
	TigerCannon, 
	PhantomMelee,
	PlayerMachineGun, PlayerHandgun, PlayerShotgun,
	Count,
};

struct GameMeshSpec
{
	wstring MaterialFile = L"";
	wstring MeshFile = L"";
};

struct GameDataSpec : public GameMeshSpec
{
	wstring file;

	UnitClassId Class = UnitClassId::LightMech;
	UnitTypeId Type = UnitTypeId::Grund;

	UINT Life = 3500;
	
	float MechRadius = 2.0f;
	float TurnAngle = 45.0f;
	
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

struct GameWeaponSpec : public GameMeshSpec
{
	WeaponType Type = WeaponType::TigerCannon;

	int Damage = 300;

	UINT FireCount = 1;
	float FireRange = 50.0f;
	float FireIntervalTime = 3.0f;
	float FireDelayTimeTillFirst = 0.0f;
	
	int FireHorizontalTiltAngle = 2;
	int FireVerticalTiltAngle = 1;

	bool CriticalDamageFire = true;	//랜덤으로 크리티컬 터질지 여부

	int TotalBulletCount = -1;
	bool TracerBulletFire = true;	//총알 속도 추적 여부
	float TracerBulletSpeed = 10.0f;
	float TracerBulletLength = 3.0f;	//범위
	float TracerBulletThickness = 0.7f;	//두께
	
	UINT ReloadBulletCount = 5;
	float ReloadIntervalTime = 4.0f;

	float ModelRadius = 0.0f;
	bool ModelAlone = false;
	bool ModelCount = 0;	//무기는 보통 카운트 하나임

};