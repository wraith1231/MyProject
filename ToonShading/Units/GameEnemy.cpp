#include "stdafx.h"
#include "GameEnemy.h"

#include "GameData.h"
#include "AiContext.h"
#include "AiState.h"
#include "Bullet.h"

#include "../Bounding/BoundingBox.h"
#include "../Bounding/ObjectsRay.h"

GameEnemy::GameEnemy(wstring matFile, wstring meshFile, ExecuteValues* value)
	: GameUnit(matFile, meshFile, value)
	, unitClass(UnitClassId::Unknown)
	, startAi(AiType::Search)
	, specData(NULL)
	, startAiTime(0.0f)
	, actionElapsedTime(0.0f)
	, start(0, 0, 0), rot(0, 0)
{
	CreateWeapon(L"", L"");

	//ai init
	{
		aiContext = new AiContext();

		AiState* state = NULL;
		state = new AiState();
		state->Updating = bind(&GameEnemy::OnSearch, this, placeholders::_1);
		aiSearch.first = aiContext->AddState(L"Search", state);
		aiSearch.second = state;

		state = new AiState();
		state->Updating = bind(&GameEnemy::OnMove, this, placeholders::_1);
		aiMove.first = aiContext->AddState(L"Move", state);
		aiMove.second = state;

		state = new AiState();
		state->Updating = bind(&GameEnemy::OnAttack, this, placeholders::_1);
		aiAttack.first = aiContext->AddState(L"Attack", state);
		aiAttack.second = state;

		state = new AiState();
		state->Updating = bind(&GameEnemy::OnTurnLeft, this, placeholders::_1);
		aiTurnLeft.first = aiContext->AddState(L"TurnLeft", state);
		aiTurnLeft.second = state;

		state = new AiState();
		state->Updating = bind(&GameEnemy::OnTurnRight, this, placeholders::_1);
		aiTurnRight.first = aiContext->AddState(L"TurnRight", state);
		aiTurnRight.second = state;
	}

	aiContext->StartState((UINT)startAi, startAiTime);

	D3DXVECTOR3 max, min;
	model->CheckMaxMinVer(max, min);
	box = new Objects::BoundingBox(max, min);

	bullet = new Bullet();
}

GameEnemy::~GameEnemy()
{
	SAFE_DELETE(bullet);
}

void GameEnemy::Update()
{
	bullet->Update();
	aiContext->Update();
	GameUnit::Update();
	D3DXMATRIX transformed = Transformed();
	D3DXMATRIX R;
	D3DXMatrixRotationX(&R, (float)D3DX_PI / 2);
	transformed = R * transformed;
	
	box->Update(transformed);
}

void GameEnemy::EditUpdate()
{
	Rotate(rot);
	rot = D3DXVECTOR2(0.0f, 0.0f);

	D3DXMATRIX transformed = Transformed();

	model->CopyAbsoluteBoneTo(transformed, boneTransforms);
}

void GameEnemy::NormalRender()
{
	bullet->NormalRender();
	GameUnit::NormalRender();
}

void GameEnemy::DepthRender()
{
	bullet->DepthRender();
	GameUnit::DepthRender();
}

void GameEnemy::DiffuseRender()
{
	bullet->DiffuseRender();
	GameUnit::DiffuseRender();
}

void GameEnemy::ImGuiRender()
{
	if (selected == true)
	{
		ImGui::Begin("Selected Enemy");

		float scale = Scale();
		ImGui::InputFloat("Scale", &scale);
		Scale(scale);
		ImGui::InputFloat2("Rotate", rot);
		D3DXVECTOR3 pos = Position();
		ImGui::InputFloat3("Translation", pos);
		Position(pos);

		ImGui::End();
	}
}

bool GameEnemy::SelectEnemy(Objects::Ray * ray)
{
	selected = false;
	D3DXMATRIX transformed = Transformed();
	box->Update(transformed);

	selected = box->Intersects(ray);
	return selected;
}

void GameEnemy::OnSearch(AiState* state)
{
	OnAiSearch(state);
}

void GameEnemy::OnMove(AiState* state)
{
	OnAiMove(state);
}

void GameEnemy::OnAttack(AiState* state)
{
	OnAiAttack(state);
}

void GameEnemy::OnTurnLeft(AiState* state)
{
	OnAiTurnLeft(state);
}

void GameEnemy::OnTurnRight(AiState* state)
{
	OnAiTurnRight(state);
}

void GameEnemy::StartAi(AiType type, float time)
{
	startAi = type;
	startAiTime = time;

	UINT index = -1;

	switch (type)
	{
	case GameEnemy::AiType::Search:
		index = aiSearch.first;
		break;
	case GameEnemy::AiType::Move:
		index = aiMove.first;
		break;
	case GameEnemy::AiType::Attack:
		index = aiAttack.first;
		break;
	case GameEnemy::AiType::TurnLeft:
		index = aiTurnLeft.first;
		break;
	case GameEnemy::AiType::TurnRight:
		index = aiTurnRight.first;
		break;
	}
	aiContext->Enable(true);
	aiContext->StartState(index, time);
	
}

void GameEnemy::NextAi(AiType type, float time)
{
	UINT index = -1;

	switch (type)
	{
	case GameEnemy::AiType::Search:
		index = aiSearch.first;
		break;
	case GameEnemy::AiType::Move:
		index = aiMove.first;
		break;
	case GameEnemy::AiType::Attack:
		index = aiAttack.first;
		break;
	case GameEnemy::AiType::TurnLeft:
		index = aiTurnLeft.first;
		break;
	case GameEnemy::AiType::TurnRight:
		index = aiTurnRight.first;
		break;
	}
	aiContext->Enable(true);
	aiContext->NextState(index, time);
}
