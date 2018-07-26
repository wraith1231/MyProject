#include "stdafx.h"
#include "GameSettings.h"

#include "../Objects/GameSkyBox.h"
#include "../Objects/GameWorld.h"

#include "GameEnemy.h"
#include "EnemyTank.h"

GamePlayer* GameSettings::player = NULL;

GameSettings::GameSettings()
	: skyBox(NULL), world(NULL)
	, enemy(NULL)
{
	CreateEnvironment();
	CreatePlayer();
	CreateEnemy();
}

GameSettings::~GameSettings()
{
	SAFE_DELETE(skyBox);
	SAFE_DELETE(world);

	SAFE_DELETE(enemy);
	SAFE_DELETE(player);
}

void GameSettings::Update()
{
	if (skyBox != NULL)
		skyBox->Update();
	if (world != NULL)
		world->Update();

	if (enemy != NULL)
		enemy->Update();
	if (player != NULL)
		player->Update();
}

void GameSettings::Render()
{
	if (skyBox != NULL)
		skyBox->Render();
	if (world != NULL)
		world->Render();

	if (enemy != NULL)
		enemy->Render();
	if (player != NULL)
		player->Render();
}

void GameSettings::PreRender()
{
	if (skyBox != NULL)
		skyBox->PreRender();
	if (world != NULL)
		world->PreRender();

	if (enemy != NULL)
		enemy->PreRender();
	if (player != NULL)
		player->PreRender();
}

void GameSettings::PreRender2()
{
	if (skyBox != NULL)
		skyBox->PreRender2();
	if (world != NULL)
		world->PreRender2();

	if (enemy != NULL)
		enemy->PreRender2();
	if (player != NULL)
		player->PreRender2();
}

void GameSettings::PostRender()
{
}

void GameSettings::ImguiRender()
{
}

void GameSettings::CreateEnvironment()
{
	wstring materialFile = Models + L"Grund/Grund.material";
	wstring meshFile = Models + L"Grund/Grund.mesh";
	wstring animPath = Models + L"Grund/";

	materialFile = Models + L"Stage/france.material";
	meshFile = Models + L"Stage/france.mesh";
	world = new GameWorld(materialFile, meshFile);

	materialFile = Models + L"Stage/france_sky.material";
	meshFile = Models + L"Stage/france_sky.mesh";
	skyBox = new GameSkyBox(materialFile, meshFile);

	skyBox->SetBasisPosition({ 0, 0, 0 });
}

void GameSettings::CreatePlayer()
{
	wstring materialFile = Models + L"Grund/Grund.material";
	wstring meshFile = Models + L"Grund/Grund.mesh";
	wstring animPath = Models + L"Grund/";
	player = new GamePlayer(materialFile, meshFile, animPath);
}

void GameSettings::CreateEnemy()
{
	wstring matFile = Models + L"Enemies/Tiger.material";
	wstring meshFile = Models + L"Enemies/Tiger.mesh";
	enemy = new EnemyTank(matFile, meshFile);

	enemy->StartAi(GameEnemy::AiType::Move, 5.0f);
	enemy->SetPosition(D3DXVECTOR3(0, 0, 30));
	D3DXMATRIX R;
	D3DXMatrixRotationY(&R, Math::ToRadian(180));
	enemy->RootAxis(R);
}
