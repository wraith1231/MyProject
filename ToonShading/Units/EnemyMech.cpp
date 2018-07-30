#include "stdafx.h"
#include "EnemyMech.h"

#include "AiState.h"
#include "AiContext.h"
#include "GameSettings.h"
#include "GameData.h"
#include "GameWeapon.h"
#include "Bullet.h"

/*
현재 대기업들 시험이 영어로 나옴
+ 자료구조 문제가 대부분 - 기본적으로 알아야 되는건 복잡도, 트리, 그래프, 탐색/정렬
트리에선 이진트리 - 에서도 힙, 그래프랑 연관되는 최소비용신장트리(Prim, Kruskal),
순회(순회방법은 연산 순서랑도 관련되어있음) / 그래프에선 너비 우선, 깊이 우선 -
트리는 선형, 그래프는 비선형이란게 차이점 - 그래프는 순환이 가능해서 많이 생김 / 
너비 우선에선 최소 길찾기(다익스투라, 플루이드, A*, Navigation Mesh 등) / 탐색정렬에선
레드블랙 트리같은거 다 나옴 비정렬된 데이터 탐색문제같은 문제도 나옴 - 테이블 만들 수 있을때
제일 빠르고 쉬운건 해쉬나 키를 테이블로 옮기면서 정렬하는 것, 키가 문자열이라면 레드블랙 같은
그런 문제들 많이 있음 - 자료구조 책 말고 자료구조를 쓰는 데이터베이스쪽에 잘 나와있음
정렬은 정렬 알고리즘 - 보통 퀵이나 기스정렬 / + 자료구조에서 수열 문제 자주 물어봄
알고리즘 풀이 연습하기에는 정보올림피아드 문제가 좋다

이번주 휴가때는 지형 툴 최대한 자기한테 맞게 만들기 - 담주 되면 로봇게임 끝내고 정리된
소스를 주던가 + 뭔가를 하고 fbx 통합까지 할건데 거기서 프레임워크를 개량하든 그대로 쓰든
해서 더 발전 시키면 포폴이 될 수 있음
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
			//가장 현명하게 판단하는건 현재 aicontext를 가지고 있는데 실행 시킬때 imgui로
			//aicontext에 대한 상태 출력시키고 진행상황을 파악하는것

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
