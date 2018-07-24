#include "stdafx.h"
#include "GameEnemy.h"

#include "GameData.h"
#include "AiContext.h"

#include "../Units/Bullet.h"

#include "../Bounding/BoundingBox.h"

/*
오늘 작성할 것 두개 : AiBase, AiContext - AiBase는 행동(fire, farming, patrol 등)
AiContext는 AiBase를 받아 실제 상태를 관리할 놈
한 상태를 다음 상태로 전이시켜주는 것이 FSM(유한 상태 기계?) / Patrol에서 Chase, 다시 Patrol 혹은
Attack, 여기서 Patrol이 바로 Attack은 하면 안됨, 가능한 단계를 다 거치도록 함 - 근데 한계가 있어서
상태에 대한걸 프로그래머가 짜고 상태를 관리하는 script를 기획이 짰음 - 여기서 얽히고 설키고 하면
문제가 많아서 FSM은 잘 안쓰고 Behavior Tree를 이용 - 언리얼도 이것, 근데 유니티는 FSM(Asset으로
Behavior Tree 이용 가능)
당장은 FSM을 이용하다 Behavior Tree로 넘어가는게 좋음
*/

GameEnemy::GameEnemy(wstring matFile, wstring meshFile)
	: GameUnit(matFile, meshFile)
	, unitClass(UnitClassId::Unknown)
	, startAi(AiType::Search)
	, specData(NULL)
	, startAiTime(0.0f)
	, actionElapsedTime(0.0f)
{
	playerBullet = NULL;

	aiContext = new AiContext();

	D3DXVECTOR3 ver[2];	//0 = min, 1 = max
	ver[1] = D3DXVECTOR3(-9999, -9999, -9999);
	ver[0] = -ver[1];
	model->CheckMaxMinVer(ver[1], ver[0]);

	box = new Objects::BoundingBox(ver[1], ver[0]);

	AddClip(Models + L"Grund/" + L"Low_Run.anim");
	AddClip(Models + L"Grund/" + L"Up_Run.anim");

	Play(0, AnimationPlayMode::Repeat);
	Play(1, AnimationPlayMode::Repeat);

	Position(D3DXVECTOR3(5, 0, 5));

	destinations.push_back(D3DXVECTOR3(5, 0, 5));
	destinations.push_back(D3DXVECTOR3(2, 0, 8));
	destinations.push_back(D3DXVECTOR3(-3, 0, 4));
	destinations.push_back(D3DXVECTOR3(-6, 0, 1));
	destinations.push_back(D3DXVECTOR3(-2, 0, -4));
	destinations.push_back(D3DXVECTOR3(1, 0, -5));
	destinations.push_back(D3DXVECTOR3(3, 0, -1));
	destinations.push_back(D3DXVECTOR3(4, 0, 2));
	destIndex = 0;

	splineTime = 0.0f;
}

GameEnemy::~GameEnemy()
{
	SAFE_DELETE(box);
}

void GameEnemy::Update()
{
	D3DXVECTOR3 v1, v2, v3, v4;
	D3DXVECTOR3 pos, prevPos;
	
	prevPos = Position();
	splineTime += Time::Delta() * 0.8f;
	
	v1 = destinations[(destIndex + 0) % destinations.size()];
	v2 = destinations[(destIndex + 1) % destinations.size()];
	v3 = destinations[(destIndex + 2) % destinations.size()];
	v4 = destinations[(destIndex + 3) % destinations.size()];
	
	D3DXVec3CatmullRom(&pos, &v1, &v2, &v3, &v4, splineTime);
	
	if (splineTime >= 1.0f)
	{
		destIndex ++;
		if (destIndex >= destinations.size())
			destIndex -= destinations.size();
		splineTime = 0.0f;
	}
	
	Position(pos);
	
	D3DXVECTOR3 temp = pos - prevPos;
	D3DXVec3Normalize(&temp, &temp);
	float rotY = atan2(temp.x, temp.z);
	
	D3DXMATRIX R;
	D3DXMatrixRotationY(&R, rotY);
	rootAxis = R;

	GameUnit::Update();
	D3DXMATRIX transformed = Transformed();

	model->CopyAbsoluteBoneTo(transformed, boneTransforms);

	if (box != NULL)
	{
		box->Update(transformed);
		//box->SetMatrix(boneTransforms[1]);
	}

	if (playerBullet != NULL)
	{
		box->SetColor(D3DXCOLOR(0, 0, 1, 1));

		if (playerBullet->CheckIntersect(box) == true)
			box->SetColor(D3DXCOLOR(0, 1, 0, 1));
	}
}

void GameEnemy::Render()
{
	if (box != NULL)
		box->Render();

	GameUnit::Render();
}

void GameEnemy::PreRender()
{
	GameUnit::PreRender();
}

void GameEnemy::PreRender2()
{
	GameUnit::PreRender2();
}

void GameEnemy::ImGuiRender()
{
}

void GameEnemy::OnSearch()
{
	OnAiSearch(aiSearch.second);
}

void GameEnemy::OnMove()
{
	OnAiMove(aiMove.second);
}

void GameEnemy::OnAttack()
{
	OnAiAttack(aiAttack.second);
}

void GameEnemy::OnTurnLeft()
{
	OnAiTurnLeft(aiTurnLeft.second);
}

void GameEnemy::OnTurnRight()
{
	OnAiTurnRight(aiTurnRight.second);
}
