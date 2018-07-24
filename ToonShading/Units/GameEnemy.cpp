#include "stdafx.h"
#include "GameEnemy.h"

#include "GameData.h"
#include "AiContext.h"

#include "../Units/Bullet.h"

#include "../Bounding/BoundingBox.h"

/*
���� �ۼ��� �� �ΰ� : AiBase, AiContext - AiBase�� �ൿ(fire, farming, patrol ��)
AiContext�� AiBase�� �޾� ���� ���¸� ������ ��
�� ���¸� ���� ���·� ���̽����ִ� ���� FSM(���� ���� ���?) / Patrol���� Chase, �ٽ� Patrol Ȥ��
Attack, ���⼭ Patrol�� �ٷ� Attack�� �ϸ� �ȵ�, ������ �ܰ踦 �� ��ġ���� �� - �ٵ� �Ѱ谡 �־
���¿� ���Ѱ� ���α׷��Ӱ� ¥�� ���¸� �����ϴ� script�� ��ȹ�� ®�� - ���⼭ ������ ��Ű�� �ϸ�
������ ���Ƽ� FSM�� �� �Ⱦ��� Behavior Tree�� �̿� - �𸮾� �̰�, �ٵ� ����Ƽ�� FSM(Asset����
Behavior Tree �̿� ����)
������ FSM�� �̿��ϴ� Behavior Tree�� �Ѿ�°� ����
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
