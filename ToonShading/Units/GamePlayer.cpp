#include "stdafx.h"
#include "GamePlayer.h"

#include "UnitPlayerInput.h"
#include "GameData.h"
#include "Bullet.h"
#include "GameWeapon.h"

#include "../Bounding/BoundingBox.h"

GamePlayer::GamePlayer(wstring matFile, wstring meshFile, wstring animPath, ExecuteValues* values)
	: GameUnit(matFile, meshFile), values(values), start(0, 0, 0)
{
	values->MainCamera->SetTarget(&position);

	LoadAnimation(animPath);
	CreateWeapon(L"", L"");

	specData = new GamePlayerSpec();
	input = new GamePlayerInput();

	bOverwriteLowerAction = false;
	bOverwriteUpperAction = false;
	bActiveBooster = false;
	bRun = true;

	actionElapsedTime = 0.0f;
	moveElapsedTime = 0.0f;
	rootRotationAngle = 0.0f;
	rootElapsedAngle = 0.0f;
	currentWaistAngle = 0.0f;
	
	moveDirection = D3DXVECTOR3(0, 0, 0);

	boneWaist = model->Bone(L"Spine");
	boneWaistSrc = boneWaist->Transform();

	bullet = new Bullet(values);

	weaponDelay = 0.0f;
	weaponTime = 1.0f;

	D3DXVECTOR3 max, min;
	model->CheckMaxMinVer(max, min);
	box = new Objects::BoundingBox(max, min);

	PlayLowerAction(LowerAction::Idle);
	PlayUpperAction(UpperAction::Idle);
}


GamePlayer::~GamePlayer()
{
	SAFE_DELETE(bullet);
	SAFE_DELETE(specData);
	SAFE_DELETE(input);
	SAFE_DELETE(bullet);
}

void GamePlayer::Update()
{
	HandleInput();

	if (prepareLowerAction != LowerAction::Unknown || bOverwriteLowerAction == true)
		PlayLowerAction(prepareLowerAction);

	if (prepareUpperAction != UpperAction::Unknown || bOverwriteUpperAction == true)
		PlayUpperAction(prepareUpperAction);

	GameUnit::Update();

	ActionRotation();

	bullet->Update();
	D3DXMATRIX transformed = Transformed();
	box->Update(transformed);
}

void GamePlayer::EditUpdate()
{
	//GameUnit::Update();

	D3DXMATRIX transformed = Transformed();
	box->Update(transformed);
}

void GamePlayer::ShadowRender()
{
	bullet->ShadowRender();
	GameUnit::ShadowRender();
}

void GamePlayer::PreRender()
{
	bullet->PreRender();
	GameUnit::PreRender();
}

void GamePlayer::ImGuiRender()
{
}

void GamePlayer::BulletClear()
{
	bullet->BulletClear();
}

void GamePlayer::SetCamTarget()
{
	values->MainCamera->SetTarget(&position);
}

void GamePlayer::LoadAnimation(wstring path)
{
	UINT lowerAnimCount = (UINT)LowerAction::Count;
	UINT upperAnimCount = (UINT)UpperAction::Count;

	indexLowerAnimations.assign((UINT)LowerAction::Count, UINT());
	indexUpperAnimations.assign((UINT)UpperAction::Count, UINT());
	//indexLowerAnimation = new UINT[lowerAnimCount];
	//indexUpperAnimation = new UINT[upperAnimCount];
	//Lower Animation
	{
		indexLowerAnimations[(UINT)LowerAction::Idle] = AddClip(path + L"Low_Idle.anim");
		indexLowerAnimations[(UINT)LowerAction::Run] = AddClip(path + L"Low_Run.anim");
		indexLowerAnimations[(UINT)LowerAction::Damage] = AddClip(path + L"Low_Damage.anim");
		indexLowerAnimations[(UINT)LowerAction::Walk] = AddClip(path + L"Low_Walk.anim");
		indexLowerAnimations[(UINT)LowerAction::LeftTurn] = AddClip(path + L"Low_TurnLeft.anim");
		indexLowerAnimations[(UINT)LowerAction::RightTurn] = AddClip(path + L"Low_TurnRight.anim");
		indexLowerAnimations[(UINT)LowerAction::BackWalk] = AddClip(path + L"Low_WalkBack.anim");
		indexLowerAnimations[(UINT)LowerAction::ForwardDead] = AddClip(path + L"Low_DeathFront.anim");
		indexLowerAnimations[(UINT)LowerAction::BackwardDead] = AddClip(path + L"Low_DeathBack.anim");
		indexLowerAnimations[(UINT)LowerAction::LeftDead] = AddClip(path + L"Low_DeathLeft.anim");
		indexLowerAnimations[(UINT)LowerAction::RightDead] = AddClip(path + L"Low_DeathRight.anim");

		indexLowerAnimations[(UINT)LowerAction::BoosterPrepare] = AddClip(path + L"Low_BoosterStart.anim");
		indexLowerAnimations[(UINT)LowerAction::BoosterActive] = AddClip(path + L"Low_BoosterMove.anim");
		indexLowerAnimations[(UINT)LowerAction::BoosterFinish] = AddClip(path + L"Low_BoosterEnd.anim");
		indexLowerAnimations[(UINT)LowerAction::BoosterBreak] = AddClip(path + L"Low_BoosterMiss.anim");
		indexLowerAnimations[(UINT)LowerAction::BoosterLeftTurn] = AddClip(path + L"Low_BoosterTurnLeft.anim");
		indexLowerAnimations[(UINT)LowerAction::BoosterRightTurn] = AddClip(path + L"Low_BoosterTurnRight.anim");
	}

	//Upper Animation
	{
		indexUpperAnimations[(UINT)UpperAction::Idle] = AddClip(path + L"Up_Idle.anim");
		indexUpperAnimations[(UINT)UpperAction::Run] = AddClip(path + L"Up_Run.anim");
		indexUpperAnimations[(UINT)UpperAction::Damage] = AddClip(path + L"Up_Damage.anim");
		indexUpperAnimations[(UINT)UpperAction::WeaponChange] = AddClip(path + L"Up_Change.anim");
						   
		indexUpperAnimations[(UINT)UpperAction::ForwardNonFire] = AddClip(path + L"Up_NonAttackFront.anim");
		indexUpperAnimations[(UINT)UpperAction::LeftNonFire] = AddClip(path + L"Up_NonAttackLeft.anim");
		indexUpperAnimations[(UINT)UpperAction::RightNonFire] = AddClip(path + L"Up_NonAttackRight.anim");
						   
		indexUpperAnimations[(UINT)UpperAction::ForwardMachineGunFire] = AddClip(path + L"Up_AttackFrontBase.anim");
		indexUpperAnimations[(UINT)UpperAction::LeftMachineGunFire] = AddClip(path + L"Up_AttackLeftBase.anim");
		indexUpperAnimations[(UINT)UpperAction::RightMachineGunFire] = AddClip(path + L"Up_AttackRightBase.anim");
		indexUpperAnimations[(UINT)UpperAction::ReloadMachineGun] = AddClip(path + L"Up_ReloadBase.anim");
						   
		indexUpperAnimations[(UINT)UpperAction::ForwardShotgunFire] = AddClip(path + L"Up_AttackFrontShotgun.anim");
		indexUpperAnimations[(UINT)UpperAction::LeftShotgunFire] = AddClip(path + L"Up_AttackLeftShotgun.anim");
		indexUpperAnimations[(UINT)UpperAction::RightShotgunFire] = AddClip(path + L"Up_AttackRightShotgun.anim");
		indexUpperAnimations[(UINT)UpperAction::ReloadShotgun] = AddClip(path + L"Up_ReloadShotgun.anim");
						   
		indexUpperAnimations[(UINT)UpperAction::ForwardHandgunFire] = AddClip(path + L"Up_AttackFrontHandgun.anim");
		indexUpperAnimations[(UINT)UpperAction::LeftHandgunFire] = AddClip(path + L"Up_AttackLeftHandgun.anim");
		indexUpperAnimations[(UINT)UpperAction::RightHandgunFire] = AddClip(path + L"Up_AttackRightHandgun.anim");
		indexUpperAnimations[(UINT)UpperAction::ReloadHandgun] = AddClip(path + L"Up_ReloadHandgun.anim");
						   
		indexUpperAnimations[(UINT)UpperAction::ForwardDead] = AddClip(path + L"Up_DeathFront.anim");
		indexUpperAnimations[(UINT)UpperAction::BackwardDead] = AddClip(path + L"Up_DeathBack.anim");
		indexUpperAnimations[(UINT)UpperAction::LeftDead] = AddClip(path + L"Up_DeathLeft.anim");
		indexUpperAnimations[(UINT)UpperAction::RightDead] = AddClip(path + L"Up_DeathRight.anim");
						   
		indexUpperAnimations[(UINT)UpperAction::BoosterPrepare] = AddClip(path + L"Up_BoosterStart.anim");
		indexUpperAnimations[(UINT)UpperAction::BoosterActive] = AddClip(path + L"Up_BoosterMove.anim");
		indexUpperAnimations[(UINT)UpperAction::BoosterFinish] = AddClip(path + L"Up_BoosterEnd.anim");
		indexUpperAnimations[(UINT)UpperAction::BoosterBreak] = AddClip(path + L"Up_BoosterMiss.anim");
		indexUpperAnimations[(UINT)UpperAction::BoosterLeftTurn] = AddClip(path + L"Up_BoosterTurnLeft.anim");
		indexUpperAnimations[(UINT)UpperAction::BoosterRightTurn] = AddClip(path + L"Up_BoosterTurnRight.anim");
	}

	runDurationTime = GetClip(indexLowerAnimations[(UINT)LowerAction::Run])->Duration;
	walkDurationTime = GetClip(indexLowerAnimations[(UINT)LowerAction::Walk])->Duration;
	boosterFinishDurationTime = GetClip(indexLowerAnimations[(UINT)LowerAction::BoosterFinish])->Duration;
	boosterBreakDurationTime = GetClip(indexLowerAnimations[(UINT)LowerAction::BoosterBreak])->Duration;
	weaponChangeDurationTime = GetClip(indexUpperAnimations[(UINT)UpperAction::WeaponChange])->Duration;
}

void GamePlayer::HandleInput()
{
	HandleMove();
	HandleRotation();
	HandleFire();
}
void GamePlayer::HandleMove()
{

	bool goingOn = false;
	D3DXVECTOR3 vel(0, 0, 0);

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// 이동
	//////////////////////////////////////////////////////////////////////////////////////////////////
	if (bActiveBooster == true)
	{

	}	//active booster true

	else
	{
		if (input->IsPressMovement() == true)
		{
			float moveSpeed = specData->WalkSpeed;

			if (bRun == true)
				moveSpeed = specData->RunSpeed;

			if (input->Pressed(GamePlayerKey::MoveForward))
			{
				if (input->Pressed(GamePlayerKey::MoveLeft))
					moveDirection = D3DXVECTOR3(-1, 0, 1);
				else if (input->Pressed(GamePlayerKey::MoveRight))
					moveDirection = D3DXVECTOR3(1, 0, 1);
				else
					moveDirection = D3DXVECTOR3(0, 0, 2);

				D3DXVec3Normalize(&moveDirection, &moveDirection);
				vel = moveDirection * (moveSpeed * (D3DXVec3Length(&moveDirection)) * 0.5f);
			}	//if forward

			else if (input->Pressed(GamePlayerKey::MoveBackward))
			{
				moveSpeed = specData->WalkBackwardSpeed;

				if (input->Pressed(GamePlayerKey::MoveLeft))
					moveDirection = D3DXVECTOR3(-1, 0, -1);
				else if (input->Pressed(GamePlayerKey::MoveRight))
					moveDirection = D3DXVECTOR3(1, 0, -1);
				else
					moveDirection = D3DXVECTOR3(0, 0, -2);

				D3DXVec3Normalize(&moveDirection, &moveDirection);
				vel = moveDirection * (moveSpeed * (D3DXVec3Length(&moveDirection)) * 0.5f);
			}	//else if backward

			else
			{
				if (input->Pressed(GamePlayerKey::MoveLeft))
					moveDirection = D3DXVECTOR3(-1, 0, 0);
				else if (input->Pressed(GamePlayerKey::MoveRight))
					moveDirection = D3DXVECTOR3(1, 0, 0);
				else
					moveDirection = D3DXVECTOR3(0, 0, 0);

				D3DXVec3Normalize(&moveDirection, &moveDirection);
				vel = moveDirection * (moveSpeed * (D3DXVec3Length(&moveDirection)) * 0.5f);
			}	//else

			goingOn = true;
		}	//if press movement // true

		else
		{
			moveDirection = D3DXVECTOR3(0, 0, 0);
			goingOn = false;
		}//press movment false

	}	//active booster false

	if (goingOn == true)
	{
		Velocity(vel);
	}
	else
	{
		Velocity(D3DXVECTOR3(0, 0, 0));
	}

	ActionMovement(vel);
}

void GamePlayer::HandleRotation()
{
	if (input->IsPressTurn() == true)
	{
		//회전 속도
		float angle = specData->TurnAngle;

		if (input->Pressed(GamePlayerKey::TurnLeft) == true)
		{
			if (rootElapsedAngle != 0.0f)
				angle += rootElapsedAngle;
			Rotate(D3DXVECTOR2(-angle, 0.0f));

			bool b = false;
			b |= currentLowerAction == LowerAction::Idle;
			b |= currentLowerAction == LowerAction::RightTurn;

			if (b == true)
				prepareLowerAction = LowerAction::LeftTurn;

		}
		else if (input->Pressed(GamePlayerKey::TurnRight) == true)
		{
			Rotate(D3DXVECTOR2(angle, 0.0f));

			bool b = false;
			b |= currentLowerAction == LowerAction::Idle;
			b |= currentLowerAction == LowerAction::LeftTurn;

			if (b == true)
				prepareLowerAction = LowerAction::RightTurn;
		}
	}
	else
	{
		bool b = false;
		b |= currentLowerAction == LowerAction::LeftTurn;
		b |= currentLowerAction == LowerAction::RightTurn;

		//원래 상태관리는 if로 안하는데 설명을 위해 if 사용

		if (b == true)
			prepareLowerAction = LowerAction::Idle;
	}


	//TODO: 뛰다 회전하면 확 틀어지는거 수정
}

void GamePlayer::HandleFire()
{
	weaponDelay += Time::Delta();
	if (input->Pressed(GamePlayerKey::WeaponFire) && weaponDelay >= weaponTime)
	{
		weaponDelay = 0.0f;
		float x = boneTransforms[15]._41;
		float y = boneTransforms[15]._42;
		float z = boneTransforms[15]._43;

		GameWeaponSpec* spec = currentWeapon->SpecData();
		bullet->AddBullet(D3DXVECTOR3(x, y, z), Direction(), spec->TracerBulletSpeed, spec->FireRange);

		x = boneTransforms[25]._41;
		y = boneTransforms[25]._42;
		z = boneTransforms[25]._43;

		bullet->AddBullet(D3DXVECTOR3(x, y, z), Direction(), spec->TracerBulletSpeed, spec->FireRange, true, D3DXVECTOR3(0, -1, 0));

	}
}

#if 1
void GamePlayer::ActionRotation()
{
	if (rootRotationAngle > currentWaistAngle)
		rootElapsedAngle = Math::Clamp(3.0f, 0.0f, rootRotationAngle - currentWaistAngle);
	else if (rootRotationAngle < currentWaistAngle)
		rootElapsedAngle = Math::Clamp(-3.0f, rootRotationAngle - currentWaistAngle, 0.0f);
	else
		rootElapsedAngle = 0.0f;

	bool b = false;
	b |= currentUpperAction == UpperAction::ForwardMachineGunFire;
	b |= currentUpperAction == UpperAction::ForwardShotgunFire;
	b |= currentUpperAction == UpperAction::ForwardHandgunFire;
	b |= currentUpperAction == UpperAction::ForwardNonFire;
	b |= currentUpperAction == UpperAction::LeftMachineGunFire;
	b |= currentUpperAction == UpperAction::LeftShotgunFire;
	b |= currentUpperAction == UpperAction::LeftHandgunFire;
	b |= currentUpperAction == UpperAction::LeftNonFire;
	b |= currentUpperAction == UpperAction::RightMachineGunFire;
	b |= currentUpperAction == UpperAction::RightShotgunFire;
	b |= currentUpperAction == UpperAction::RightHandgunFire;
	b |= currentUpperAction == UpperAction::RightNonFire;

	if (b == true)
	{
		currentWaistAngle = -rootElapsedAngle;
	}

	b = false;
	b |= currentLowerAction == LowerAction::Idle;
	b |= currentLowerAction == LowerAction::Damage;
	b |= currentLowerAction == LowerAction::ForwardDead;
	b |= currentLowerAction == LowerAction::BackwardDead;
	b |= currentLowerAction == LowerAction::LeftDead;
	b |= currentLowerAction == LowerAction::RightDead;

	if (b == true)
	{
		rootRotationAngle = 0.0f;
		rootElapsedAngle = 0.0f;

		D3DXMATRIX T;
		D3DXMatrixIdentity(&T);
		rootAxis = T;
	}
	else
	{
		{
			currentWaistAngle += rootElapsedAngle;

			D3DXMATRIX R;
			D3DXMatrixRotationY(&R, Math::ToRadian(currentWaistAngle));
			rootAxis = R;
		}
	}
	D3DXMATRIX transformed = Transformed();
	model->CopyAbsoluteBoneTo(transformed, boneTransforms);
}
#endif
#pragma region 내 전용 ActionRotation
#if 0
void GamePlayer::ActionRotation()
{
	if (rootRotationAngle > 0.0f)
	{
		//Right Side Rotation
		if (rootRotationAngle != rootElapsedAngle)
			rootElapsedAngle = Math::Clamp(rootElapsedAngle, 0.0f, rootRotationAngle);

	}	//if (rootRotationAngle > 0.0f)
	else if (rootRotationAngle < 0.0f)
	{
		//Left Side Rotation
		if(rootRotationAngle != rootElapsedAngle)
			rootElapsedAngle = Math::Clamp(rootElapsedAngle, rootRotationAngle, 0.0f);

	}	//else if (rootRotationAngle < 0.0f)
	else
	{
		//Return Forward
		rootElapsedAngle = 0.0f;

	}	//else

	float currentWaistAngle = 0.0f;
	bool b1 = false;
	b1 |= currentUpperAction == UpperAction::ForwardMachineGunFire;
	b1 |= currentUpperAction == UpperAction::ForwardShotgunFire;
	b1 |= currentUpperAction == UpperAction::ForwardHandgunFire;
	b1 |= currentUpperAction == UpperAction::ForwardNonFire;
	b1 |= currentUpperAction == UpperAction::LeftMachineGunFire;
	b1 |= currentUpperAction == UpperAction::LeftShotgunFire;
	b1 |= currentUpperAction == UpperAction::LeftHandgunFire;
	b1 |= currentUpperAction == UpperAction::LeftNonFire;
	b1 |= currentUpperAction == UpperAction::RightMachineGunFire;
	b1 |= currentUpperAction == UpperAction::RightShotgunFire;
	b1 |= currentUpperAction == UpperAction::RightHandgunFire;
	b1 |= currentUpperAction == UpperAction::RightNonFire;

	if (b1 == true)
	{
		if(values == NULL)
		currentWaistAngle = -rootElapsedAngle;
		else
		{
			D3DXVECTOR2 rot;
			values->MainCamera->GetRotation(&rot);
			currentWaistAngle = rot.y * 180 / (float)D3DX_PI;
		}
	}

	bool b2 = false;
	b2 |= currentLowerAction == LowerAction::Idle;
	b2 |= currentLowerAction == LowerAction::Damage;
	b2 |= currentLowerAction == LowerAction::ForwardDead;
	b2 |= currentLowerAction == LowerAction::BackwardDead;
	b2 |= currentLowerAction == LowerAction::LeftDead;
	b2 |= currentLowerAction == LowerAction::RightDead;

	if (b2 == true)
	{
		rootRotationAngle = 0.0f;
		rootElapsedAngle = 0.0f;

		D3DXMATRIX T;
		D3DXMatrixIdentity(&T);
		rootAxis = T;

		D3DXVECTOR3 right;
		right.x = boneWaist->Transform()._11;
		right.y = boneWaist->Transform()._12;
		right.z = boneWaist->Transform()._13;
		D3DXMATRIX R;
		D3DXMatrixRotationAxis(&R, &right, Math::ToRadian(rootElapsedAngle));
		D3DXMATRIX temp = boneWaist->Transform();
		boneWaist->Transform(temp * R);
	}
	else
	{
		if (currentWaistAngle != 0.0f || rootElapsedAngle != 0.0f)
		{
			if(b1 == true)
				currentWaistAngle -= rootElapsedAngle;

			D3DXMATRIX R;
			D3DXMatrixRotationY(&R, Math::ToRadian(rootElapsedAngle));
			rootAxis = R;

			D3DXVECTOR3 right;
			right.x = boneWaist->Transform()._11;
			right.y = boneWaist->Transform()._12;
			right.z = boneWaist->Transform()._13;
			D3DXMatrixRotationAxis(&R, &right, Math::ToRadian(currentWaistAngle));

			D3DXMATRIX temp = boneWaist->Transform();
			boneWaist->Transform(temp * R);
		}
	}
	D3DXMATRIX transformed = Transformed();
	model->CopyAbsoluteBoneTo(transformed, boneTransforms);

}
#endif
#pragma endregion

void GamePlayer::PlayLowerAction(LowerAction action)
{
	PlayLowerAction(action, 0.0f);
}

void GamePlayer::PlayLowerAction(LowerAction action, float startTime)
{
	AnimationPlayMode mode = AnimationPlayMode::Repeat;
	float blendTime = 0.0f;

	switch (action)
	{
	case LowerAction::Idle:
		blendTime = 0.5f;
		break;
	case LowerAction::Walk:
		blendTime = 0.3f;
		break;
	case LowerAction::Run:
		blendTime = 0.3f;
		break;
	case LowerAction::BackWalk:
		blendTime = 0.3f;
		break;
	case LowerAction::Damage:
		mode = AnimationPlayMode::Once;
		blendTime = 0.2f;
		break;
	case LowerAction::LeftTurn:
	case LowerAction::RightTurn:
		blendTime = 0.5f;
		break;
	case LowerAction::ForwardDead:
	case LowerAction::BackwardDead:
	case LowerAction::LeftDead:
	case LowerAction::RightDead:
		mode = AnimationPlayMode::Once;
		blendTime = 0.2f;
		break;
	case LowerAction::BoosterPrepare:
	case LowerAction::BoosterActive:
	case LowerAction::BoosterFinish:
	case LowerAction::BoosterBreak:
	case LowerAction::BoosterLeftTurn:
	case LowerAction::BoosterRightTurn:
		mode = AnimationPlayMode::Once;
		blendTime = 0.2f;
		break;
	default:
		//원래는 assert 걸어줘야하는데 지금은 패스
		break;
	}

	Play(indexLowerAnimations[(UINT)action], startTime, blendTime, 1.0f, mode);

	currentLowerAction = action;
	prepareLowerAction = LowerAction::Unknown;
	bOverwriteLowerAction = false;
}

void GamePlayer::PlayUpperAction(UpperAction action)
{
	PlayUpperAction(action, 0.0f);
}

void GamePlayer::PlayUpperAction(UpperAction action, float startTime)
{
	AnimationPlayMode mode = AnimationPlayMode::Repeat;
	float blendTime = 0.0f;

	switch (action)
	{
	case GamePlayer::UpperAction::Idle:
		blendTime = 0.5f;
		break;
	case GamePlayer::UpperAction::Run:
		blendTime = 0.3f;
		break;
	case GamePlayer::UpperAction::Damage:
		mode = AnimationPlayMode::Once;
		blendTime = 0.2f;
		break;
	case GamePlayer::UpperAction::WeaponChange:
		mode = AnimationPlayMode::Once;
		blendTime = 0.2f;
		break;
	case GamePlayer::UpperAction::ForwardNonFire:
	case GamePlayer::UpperAction::LeftNonFire:
	case GamePlayer::UpperAction::RightNonFire:
		//탄환 없음
		mode = AnimationPlayMode::Once;
		blendTime = 0.5f;
		break;
	case GamePlayer::UpperAction::ForwardMachineGunFire:
	case GamePlayer::UpperAction::LeftMachineGunFire:
	case GamePlayer::UpperAction::RightMachineGunFire:
	case GamePlayer::UpperAction::ForwardShotgunFire:
	case GamePlayer::UpperAction::LeftShotgunFire:
	case GamePlayer::UpperAction::RightShotgunFire:
	case GamePlayer::UpperAction::ForwardHandgunFire:
	case GamePlayer::UpperAction::LeftHandgunFire:
	case GamePlayer::UpperAction::RightHandgunFire:
		mode = AnimationPlayMode::Once;
		blendTime = 0.5f;
		break;
	case GamePlayer::UpperAction::ReloadMachineGun:
	case GamePlayer::UpperAction::ReloadShotgun:
	case GamePlayer::UpperAction::ReloadHandgun:
		mode = AnimationPlayMode::Once;
		blendTime = 0.5f;
		break;
	case GamePlayer::UpperAction::ForwardDead:
	case GamePlayer::UpperAction::BackwardDead:
	case GamePlayer::UpperAction::LeftDead:
	case GamePlayer::UpperAction::RightDead:
		mode = AnimationPlayMode::Once;
		blendTime = 0.2f;
		break;
	case GamePlayer::UpperAction::BoosterPrepare:
	case GamePlayer::UpperAction::BoosterActive:
	case GamePlayer::UpperAction::BoosterFinish:
	case GamePlayer::UpperAction::BoosterBreak:
	case GamePlayer::UpperAction::BoosterLeftTurn:
	case GamePlayer::UpperAction::BoosterRightTurn:
		mode = AnimationPlayMode::Once;
		blendTime = 0.2f;
		break;
	default:
		break;
	}

	Play(indexUpperAnimations[(UINT)action], startTime, blendTime, 1.0f, mode);

	currentUpperAction = action;
	prepareUpperAction = UpperAction::Unknown;
	bOverwriteUpperAction = false;
}

void GamePlayer::ActionMovement(D3DXVECTOR3 direction)
{
	bool bMove = true;

	if (direction.z > 0.0f)
	{
		if (direction.x < 0.0f)
			rootRotationAngle = -45.0f;
		else if (direction.x > 0.0f)
			rootRotationAngle = 45.0f;
		else
			rootRotationAngle = 0.0f;

		if (bRun == true)
		{
			if (currentLowerAction != LowerAction::Run)
				prepareLowerAction = LowerAction::Run;
		}
		else
		{
			if (currentLowerAction != LowerAction::Walk)
				prepareLowerAction = LowerAction::Walk;
		}
	}	//direction.z > 0.0f
	else if(direction.z < 0.0f)
	{
		if (direction.x < 0.0f)
			rootRotationAngle = 30.0f;
		else if (direction.x > 0.0f)
			rootRotationAngle = -30.0f;
		else
			rootRotationAngle = 0.0f;

			if (currentLowerAction != LowerAction::BackWalk)
				prepareLowerAction = LowerAction::BackWalk;
	}	//direction.z < 0.0f
	else 
	{
		if (direction.x > 0.0f || direction.x < 0.0f)
		{
			if (direction.x < 0.0f)
				rootRotationAngle = -90;
			else if (direction.x > 0.0f)
				rootRotationAngle = 90;

			if (bRun == true)
			{
				if (currentLowerAction != LowerAction::Run)
					prepareLowerAction = LowerAction::Run;
			}
			else
			{
				if (currentLowerAction != LowerAction::Walk)
					prepareLowerAction = LowerAction::Walk;
			}
		}
		else
		{

			bMove = false;

			bool b = false;
			b |= currentLowerAction == LowerAction::Run;
			b |= currentLowerAction == LowerAction::Walk;
			b |= currentLowerAction == LowerAction::BackWalk;

			if (b == true)
				prepareLowerAction = LowerAction::Idle;

			rootRotationAngle = 0.0f;
			moveElapsedTime = 0.0f;
		}
	} //direction.z = 0


}
