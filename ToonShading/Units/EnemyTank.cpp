#include "stdafx.h"
#include "EnemyTank.h"

#include "AiState.h"
#include "AiContext.h"
#include "GameSettings.h"
#include "GameData.h"
#include "GameWeapon.h"

EnemyTank::EnemyTank(wstring matFile, wstring meshFile)
	: GameEnemy(matFile, meshFile)
{
	currentAction = Action::Unknown;
	engageAction = Action::Unknown;

	for (UINT i = 0; i < model->BoneCount(); i++)
	{
		ModelBone* bone = model->Bone(i);

		if (bone->Name() == L"GunDummy")
		{
			indexFireWeaponBone = bone->Index();
		}
	}

	indexTurretBone = model->Bone(L"Top")->Index();
	transformTurret = model->Bone(indexTurretBone)->Transform();

	bOverwriteAction = false;
	turretAngleSpeed = 0.0f;

	specData = new GameEnemySpec();
	specData->Type = UnitTypeId::Tiger;
	specData->Class = UnitClassId::Tank;
	specData->Life = 1300;
	specData->MechRadius = 2.8f;
	specData->MoveSpeed = 4;
	specData->TurnAngle = 20;
	
	currentWeapon->SpecData()->Type = WeaponType::TigerCannon;
	currentWeapon->SpecData()->Damage = 300;
	currentWeapon->SpecData()->FireRange = 70;
	currentWeapon->SpecData()->FireCount = 1;
	currentWeapon->SpecData()->TotalBulletCount = -1;
	currentWeapon->SpecData()->ReloadBulletCount = 5;
	currentWeapon->SpecData()->FireIntervalTime = 3;
	currentWeapon->SpecData()->ReloadIntervalTime = 4;
	currentWeapon->SpecData()->FireHorizontalTiltAngle = 2;
	currentWeapon->SpecData()->FireVerticalTiltAngle = 1;
	currentWeapon->SpecData()->FireDelayTimeTillFirst = 0;
	currentWeapon->SpecData()->CriticalDamageFire = true;
	currentWeapon->SpecData()->ModelAlone = true;
	currentWeapon->SpecData()->ModelCount = 0;
	currentWeapon->SpecData()->TracerBulletFire = true;
	currentWeapon->SpecData()->TracerBulletSpeed = 200;
	currentWeapon->SpecData()->TracerBulletLength = 3.0f;
	currentWeapon->SpecData()->TracerBulletThickness = 0.7f;

	//rotate = 0.0f;
	PlayAction(Action::Idle);
}

EnemyTank::~EnemyTank()
{
}

void EnemyTank::Update()
{
	bOverwriteAction = false;

	ProcessAction();
	
	bool b = false;
	b &= engageAction != Action::Unknown;
	b &= engageAction != currentAction;
	b |= bOverwriteAction == true;

	if (b == true)
	{
		PlayAction(engageAction);
	}

	if (turretAngleSpeed != 0.0f)
	{
		float rotate = turretAngleSpeed * Time::Delta();
		if (rotate >= 180.0f) rotate -= 360.0f;
		if (rotate <= -180.0f) rotate += 360.0f;

		D3DXVECTOR3 right;
		right.x = transformTurret._11;
		right.y = transformTurret._12;
		right.z = transformTurret._13;

		D3DXMATRIX mat;
		D3DXMatrixRotationAxis(&mat, &right, Math::ToRadian(rotate));

		transformTurret *= mat;
		//model->Bone(indexTurretBone)->Transform(transformTurret * mat);
	}

	model->Bone(indexTurretBone)->Transform(transformTurret);
	//D3DXMATRIX transformed = Transformed();
	//model->CopyAbsoluteBoneTo(transformed, boneTransforms);

	GameEnemy::Update();
}

void EnemyTank::Render()
{
	GameEnemy::Render();

	ImGui::Text("Tank State : %d, %d", (int)engageAction, (int)currentAction);
}

void EnemyTank::ActionIdle()
{
	engageAction = Action::Idle;
}

bool EnemyTank::ActionFire()
{
	D3DXVECTOR3 fireStart;	//발사 위치
	fireStart.x = boneTransforms[indexTurretBone]._41;
	fireStart.y = boneTransforms[indexTurretBone]._42;
	fireStart.z = boneTransforms[indexTurretBone]._43;

	D3DXVECTOR3 fireDir;
	//회전축 틀어져있어서 up 이용
	fireDir.x = boneTransforms[indexTurretBone]._21;
	fireDir.y = boneTransforms[indexTurretBone]._22;
	fireDir.z = boneTransforms[indexTurretBone]._23;

	//TODO: 발사

	return false;
}

void EnemyTank::ActionHit(GameUnit * attacker)
{
	if (IsDead() == true) return;

	GameWeaponSpec* specData = attacker->CurrentWeapon()->SpecData();

	GamePlayer::DebugMode mode = GameSettings::Player()->GetDebugMode();

	bool b = false;
	b |= mode == GamePlayer::DebugMode::Superman;
	b |= mode == GamePlayer::DebugMode::God;

	if (b == true)
	{
		life = 0.0f;
	}
	else
	{
		life -= specData->Damage;
		if (life < 1.0f)
			life = 0.0f;
	}

	if (life < 1.0f)
		ActionDead(attacker->Position());
}

bool EnemyTank::ActionReload(GameWeapon * weapon)
{
	//TODO : Reload Process - Need Weapon

	return false;
}

void EnemyTank::ActionDamage(GameUnit * attacker)
{
	engageAction = Action::Damage;

	actionElapsedTime = 0.0f;
}

void EnemyTank::ActionDead(D3DXVECTOR3 attackerPosition)
{
	engageAction = Action::Dead;
	Velocity(D3DXVECTOR3(0, 0, 0));

	turretAngleSpeed = 0.0f;
	aiContext->Enable(false);
}

void EnemyTank::ProcessAction()
{
	switch (currentAction)
	{
	case EnemyTank::Action::Idle:
	{

	}
		break;
	case EnemyTank::Action::Move:
	{

	}
		break;
	case EnemyTank::Action::Damage:
	{
		ActionIdle();
		actionElapsedTime = 0.0f;
	}
		break;
	case EnemyTank::Action::Fire:
	{
		if (actionElapsedTime >= currentWeapon->SpecData()->FireIntervalTime)
		{
			ActionIdle();
			actionElapsedTime = 0.0f;
		}
		else
		{
			actionElapsedTime += Time::Delta();
		}
		
	}
		break;
	case EnemyTank::Action::Reload:
	{
		if (actionElapsedTime >= currentWeapon->SpecData()->ReloadIntervalTime)
		{
			ActionIdle();
			actionElapsedTime = 0.0f;
		}
		else
		{
			actionElapsedTime += Time::Delta();
		}
	}
		break;
	case EnemyTank::Action::Dead:
	{
		//TODO: 사망처리
		//일단은 객체를 렌더링하지 않는 방식으로 처리
		//enable,  visible->false
	}
		break;
	}

	/*
	Ai가 난해하게 느껴지는 이유 - 구조 자체는 단순한 편인데 상태가 계속 왔다갔다 하면서
	여기갔다 저기갔다 하는데 상태에 따른 액션을 처리할 때 
	*/
}

void EnemyTank::OnAiSearch(AiState * state)
{
	if (state->IsActive() == false)
	{
		D3DXVECTOR3 position = GameSettings::Player()->Position();
		float distance = Math::Distance(position, this->Position());

		D3DXVECTOR3 direction;
		direction = position - this->Position();
		//direction = this->Position() - position;

		D3DXMATRIX turret = model->Bone(indexTurretBone)->Transform();
		//D3DXVECTOR3 forward(turret._31, turret._32, turret._33);
		D3DXVECTOR3 forward(turret._32, turret._31, -turret._33);

		D3DXVec3Normalize(&direction, &direction);
		D3DXVec3Normalize(&forward, &forward);
		float dot;
		dot = D3DXVec3Dot(&forward, &direction);
		float angle = Math::ToDegree(acosf(dot));
		D3DXVECTOR3 cross;
		D3DXVec3Cross(&cross, &forward, &direction);

		if (cross.y < 0.0f) angle = -angle;

		if (fabs(angle) < 10.0f)
		{
			if (distance <= currentWeapon->SpecData()->FireRange)
			{
				//TODO:Attack
				NextAi(AiType::Attack, 1.0f);
			}
			else
			{
				NextAi(AiType::Move, 1.0f);
			}
			turretAngleSpeed = 0.0f;
		}
		else
		{
			if (angle > 10.0f)
			{
				NextAi(AiType::TurnRight, fabs(angle) / specData->TurnAngle);
			}
			else if (angle < 10.0f)
			{
				NextAi(AiType::TurnLeft, fabs(angle) / specData->TurnAngle);
			}
			//가장 현명하게 판단하는건 현재 aicontext를 가지고 있는데 실행 시킬때 imgui로
			//aicontext에 대한 상태 출력시키고 진행상황을 파악하는것

		} // else

	}//bActive == false

	Velocity(D3DXVECTOR3(0, 0, 0));
}

void EnemyTank::OnAiMove(AiState * state)
{
	if (state->IsActive() == true)
	{
		D3DXVECTOR3 moveVelocity = D3DXVECTOR3(0.0f, 0.0f, -specData->MoveSpeed);

		//TODO: collision detection : obstacles
		//충돌여부 판단
		//충돌시 정지(velocity->0)
		//충돌 없음 - 이동
		
		Velocity(moveVelocity);
	}
	else
	{
		Velocity(D3DXVECTOR3(0, 0, 0));
		turretAngleSpeed = 0.0f;

		NextAi(AiType::Search, Math::Random(0.0f, 1.0f));
	}
}

void EnemyTank::OnAiAttack(AiState * state)
{
	if (state->IsActive() == true)
	{
		Velocity(D3DXVECTOR3(0, 0, 0));

		//TODO: Reload Check
		//Reload-> Reload Process

		//TODO: Check Fire Enable
		//Fire Enabled == true -> Fire
		ActionFire();

		//TODO: Attack

		turretAngleSpeed = 0.0f;
	}
	
	NextAi(AiType::Search, Math::Random(0.0f, 1.0f));
}

void EnemyTank::OnAiTurnLeft(AiState * state)
{
	if (state->IsActive() == true)
	{
		turretAngleSpeed = -specData->TurnAngle;
	}
	else
		NextAi(AiType::Search, Math::Random(0.0f, 1.0f));
}

void EnemyTank::OnAiTurnRight(AiState * state)
{
	if (state->IsActive() == true)
	{
		turretAngleSpeed = specData->TurnAngle;
	}
	else
		NextAi(AiType::Search, Math::Random(0.0f, 1.0f));
}
