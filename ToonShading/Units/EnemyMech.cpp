#include "stdafx.h"
#include "EnemyMech.h"

#include "AiState.h"
#include "AiContext.h"
#include "GameSettings.h"
#include "GameData.h"
#include "GameWeapon.h"
#include "Bullet.h"

/*
���� ������ ������ ����� ����
+ �ڷᱸ�� ������ ��κ� - �⺻������ �˾ƾ� �Ǵ°� ���⵵, Ʈ��, �׷���, Ž��/����
Ʈ������ ����Ʈ�� - ������ ��, �׷����� �����Ǵ� �ּҺ�����Ʈ��(Prim, Kruskal),
��ȸ(��ȸ����� ���� �������� ���õǾ�����) / �׷������� �ʺ� �켱, ���� �켱 -
Ʈ���� ����, �׷����� �����̶��� ������ - �׷����� ��ȯ�� �����ؼ� ���� ���� / 
�ʺ� �켱���� �ּ� ��ã��(���ͽ�����, �÷��̵�, A*, Navigation Mesh ��) / Ž�����Ŀ���
����� Ʈ�������� �� ���� �����ĵ� ������ Ž���������� ������ ���� - ���̺� ���� �� ������
���� ������ ����� �ؽ��� Ű�� ���̺�� �ű�鼭 �����ϴ� ��, Ű�� ���ڿ��̶�� ����� ����
�׷� ������ ���� ���� - �ڷᱸ�� å ���� �ڷᱸ���� ���� �����ͺ��̽��ʿ� �� ��������
������ ���� �˰��� - ���� ���̳� �⽺���� / + �ڷᱸ������ ���� ���� ���� ���
�˰��� Ǯ�� �����ϱ⿡�� �����ø��ǾƵ� ������ ����

�̹��� �ް����� ���� �� �ִ��� �ڱ����� �°� ����� - ���� �Ǹ� �κ����� ������ ������
�ҽ��� �ִ��� + ������ �ϰ� fbx ���ձ��� �Ұǵ� �ű⼭ �����ӿ�ũ�� �����ϵ� �״�� ����
�ؼ� �� ���� ��Ű�� ������ �� �� ����
*/

EnemyMech::EnemyMech(wstring matFile, wstring meshFile)
	: GameEnemy(matFile, meshFile)
	, currentAction(Action::Unknown), engageAction(Action::Unknown)
	, defaultAnimSpeed(1.0f)
	, fireDelayElapsedTime(0.0f)
	, moveDurationTime(0.0f), moveElapsedTime(0.0f)
	, bCriticalDamage(false), bMoveBlocked(false)
	, indexLeftFireWeaponBone(-1), indexRightFireWeaponBone(-1)
{
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

EnemyMech::~EnemyMech()
{
}

void EnemyMech::Update()
{
	ProcessAction();
	
	bool b = false;
	b &= engageAction != Action::Unknown;
	b &= engageAction != currentAction;

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

void EnemyMech::Render()
{
	GameEnemy::Render();

	ImGui::Text("Tank State : %d, %d", (int)engageAction, (int)currentAction);
}

void EnemyMech::ActionIdle()
{
	engageAction = Action::Idle;
}

bool EnemyMech::ActionFire()
{
	D3DXVECTOR3 fireStart;	//�߻� ��ġ
	fireStart.x = boneTransforms[indexTurretBone]._41;
	fireStart.y = boneTransforms[indexTurretBone]._42;
	fireStart.z = boneTransforms[indexTurretBone]._43;

	D3DXVECTOR3 fireDir;
	//ȸ���� Ʋ�����־ up �̿�
	fireDir.x = boneTransforms[indexTurretBone]._21;
	fireDir.y = boneTransforms[indexTurretBone]._22;
	fireDir.z = boneTransforms[indexTurretBone]._23;

	//TODO: �߻�
	if (currentWeapon == NULL)
		return false;

	if (bullet->BulletQuan() > currentWeapon->SpecData()->FireCount)
		return false;

	bullet->AddBullet(fireStart, fireDir, currentWeapon->SpecData()->TracerBulletSpeed, currentWeapon->SpecData()->FireRange);
	currentWeapon->ReloadTime(currentWeapon->SpecData()->FireIntervalTime);

	return true;
}

void EnemyMech::ActionHit(GameUnit * attacker)
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

bool EnemyMech::ActionReload(GameWeapon * weapon)
{
	//TODO : Reload Process - Need Weapon
	if (weapon == NULL)
		return false;

	float reloadTime = weapon->ReloadTime() - Time::Delta();
	weapon->ReloadTime(reloadTime);

	return true;
}

void EnemyMech::ActionDamage(GameUnit * attacker)
{
	engageAction = Action::Damage;

	actionElapsedTime = 0.0f;
}

void EnemyMech::ActionDead(D3DXVECTOR3 attackerPosition)
{
	engageAction = Action::Dead;
	Velocity(D3DXVECTOR3(0, 0, 0));

	turretAngleSpeed = 0.0f;
	aiContext->Enable(false);
}

void EnemyMech::ProcessAction()
{
	switch (currentAction)
	{
	case EnemyMech::Action::Idle:
	{

	}
		break;
	case EnemyMech::Action::Move:
	{

	}
		break;
	case EnemyMech::Action::Damage:
	{
		ActionIdle();
		actionElapsedTime = 0.0f;
	}
		break;
	case EnemyMech::Action::Fire:
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
	case EnemyMech::Action::Reload:
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
	case EnemyMech::Action::Dead:
	{
		//TODO: ���ó��
		//�ϴ��� ��ü�� ���������� �ʴ� ������� ó��
		//enable,  visible->false
	}
		break;
	}

	/*
	Ai�� �����ϰ� �������� ���� - ���� ��ü�� �ܼ��� ���ε� ���°� ��� �Դٰ��� �ϸ鼭
	���Ⱜ�� ���Ⱜ�� �ϴµ� ���¿� ���� �׼��� ó���� �� 
	*/
}

void EnemyMech::OnAiSearch(AiState * state)
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

		if (fabs(angle) < 3.0f)
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
			if (angle > 3.0f)
			{
				NextAi(AiType::TurnRight, fabs(angle) / specData->TurnAngle);
			}
			else if (angle < 3.0f)
			{
				NextAi(AiType::TurnLeft, fabs(angle) / specData->TurnAngle);
			}
			//���� �����ϰ� �Ǵ��ϴ°� ���� aicontext�� ������ �ִµ� ���� ��ų�� imgui��
			//aicontext�� ���� ���� ��½�Ű�� �����Ȳ�� �ľ��ϴ°�

		} // else

	}//bActive == false

	Velocity(D3DXVECTOR3(0, 0, 0));
}

void EnemyMech::OnAiMove(AiState * state)
{
	if (state->IsActive() == true)
	{
		D3DXVECTOR3 moveVelocity = D3DXVECTOR3(0.0f, 0.0f, -specData->MoveSpeed);

		//TODO: collision detection : obstacles
		//�浹���� �Ǵ�
		//�浹�� ����(velocity->0)
		//�浹 ���� - �̵�
		
		Velocity(moveVelocity);
	}
	else
	{
		Velocity(D3DXVECTOR3(0, 0, 0));
		turretAngleSpeed = 0.0f;

		NextAi(AiType::Search, Math::Random(0.0f, 1.0f));
	}
}

void EnemyMech::OnAiAttack(AiState * state)
{
	if (state->IsActive() == true)
	{
		Velocity(D3DXVECTOR3(0, 0, 0));

		if (currentWeapon->ReloadTime() < 0.0f)
		{
			ActionFire();
		}
		else
		{
			ActionReload(currentWeapon);
		}
		//TODO: Reload Check
		//Reload-> Reload Process

		//TODO: Check Fire Enable
		//Fire Enabled == true -> Fire
		//ActionFire();

		//TODO: Attack

		turretAngleSpeed = 0.0f;
	}
	
	NextAi(AiType::Search, Math::Random(0.0f, 1.0f));
}

void EnemyMech::OnAiTurnLeft(AiState * state)
{
	if (state->IsActive() == true)
	{
		turretAngleSpeed = -specData->TurnAngle;
	}
	else
		NextAi(AiType::Search, Math::Random(0.0f, 1.0f));
}

void EnemyMech::OnAiTurnRight(AiState * state)
{
	if (state->IsActive() == true)
	{
		turretAngleSpeed = specData->TurnAngle;
	}
	else
		NextAi(AiType::Search, Math::Random(0.0f, 1.0f));
}
