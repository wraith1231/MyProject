#include "stdafx.h"
#include "DrawModel.h"

#include "../Fbx/Exporter.h"

#include "../Units/GamePlayer.h"
#include "../Units/GameEnemy.h"
#include "../Units/Bullet.h"

#include "../Objects/GameAnimationModel.h"
#include "../Objects/GameWorld.h"
#include "../Objects/GameSkyBox.h"


/*
과제
로봇 현재 총이 없는데 찾으면 NULL로 되어있는 총기 포인트가 있음
그쪽 본(양손 두개) 찾아서 해당 본 위치에서 sphere를 전방으로 발사
적은 충돌박스를 갖고 있는데 sphere와 박스의 충돌 구현
+ 하나는 총알이 직선, 하나는 곡선으로 
+ 적의 이동 포인트를 몇개 설정해서 catmull rom 방식으로 자연스럽게 이동
캐릭터도 그걸 따라가게
*/

DrawModel::DrawModel(ExecuteValues* values)
	: Execute(values)
	, skybox(NULL), world(NULL), 
	grund(NULL), player(NULL)
{
	CreateEnvironment();
	//CreateAnimationModel();
	CreatePlayer();
	CreateEnemy();

}

DrawModel::~DrawModel()
{
	SAFE_DELETE(skybox);
	SAFE_DELETE(world);
	SAFE_DELETE(grund);
	SAFE_DELETE(player);
}

void DrawModel::Update()
{
	if(skybox != NULL)
		skybox->Update();
	if(world != NULL)
		world->Update();
	if(grund != NULL)
		grund->Update();
	if (player != NULL)
		player->Update();
	if (enemy != NULL)
		enemy->Update();
}

void DrawModel::PreRender()
{
	if (skybox != NULL)
		skybox->PreRender();
	if (world != NULL)
		world->PreRender();
	if (grund != NULL)
		grund->PreRender();
	if (player != NULL)
		player->PreRender();
	if (enemy != NULL)
		enemy->PreRender();
}

void DrawModel::PreRender2()
{
	if (skybox != NULL)
		skybox->PreRender2();
	if (world != NULL)
		world->PreRender2();
	if (grund != NULL)
		grund->PreRender2();
	if (player != NULL)
		player->PreRender2();
	if (enemy != NULL)
		enemy->PreRender2();
}

void DrawModel::Render()
{
	if (skybox != NULL)
		skybox->Render();
	if (world != NULL)
		world->Render();
	if (grund != NULL)
		grund->Render();
	if (player != NULL)
		player->Render();
	if (enemy != NULL)
		enemy->Render();
}

void DrawModel::PostRender()
{

}

void DrawModel::PostRender2()
{
}

void DrawModel::ImGuiRender()
{
}

D3DXVECTOR3 * DrawModel::GetCharPos()
{
	if(player != NULL)
		return player->FollowPos();

	if (grund != NULL)
		return grund->FollowPos();

	return NULL;
	//return grund->FollowPos();
}

void DrawModel::CreateEnvironment()
{
	wstring materialFile = Models + L"Grund/Grund.material";
	wstring meshFile = Models + L"Grund/Grund.mesh";
	wstring animPath = Models + L"Grund/";

	materialFile = Models + L"Stage/france.material";
	meshFile = Models + L"Stage/france.mesh";
	world = new GameWorld(materialFile, meshFile);

	materialFile = Models + L"Stage/france_sky.material";
	meshFile = Models + L"Stage/france_sky.mesh";
	skybox = new GameSkyBox(materialFile, meshFile);

	skybox->SetBasisPosition({ 0, 20, 0 });

}

void DrawModel::CreateAnimationModel()
{
	wstring materialFile = Models + L"Grund/Grund.material";
	wstring meshFile = Models + L"Grund/Grund.mesh";
	wstring animPath = Models + L"Grund/";
	grund = new GameAnimationModel(materialFile, meshFile);

	grund->AddClip(Models + L"Grund/Low_Idle.anim");
	grund->AddClip(Models + L"Grund/Up_Idle.anim");
	grund->AddClip(Models + L"Grund/Low_Run.anim");
	grund->AddClip(Models + L"Grund/Up_Run.anim");

	grund->Play(0, 0, 0.0f, 1.0f, AnimationPlayMode::Repeat);
	grund->Play(1, 0, 0.0f, 1.0f, AnimationPlayMode::Repeat);
}

void DrawModel::CreatePlayer()
{
	wstring materialFile = Models + L"Grund/Grund.material";
	wstring meshFile = Models + L"Grund/Grund.mesh";
	wstring animPath = Models + L"Grund/";
	player = new GamePlayer(materialFile, meshFile, animPath, values);
}

void DrawModel::CreateEnemy()
{
	wstring materialFile = Models + L"Grund/Grund.material";
	wstring meshFile = Models + L"Grund/Grund.mesh";

	enemy = new GameEnemy(materialFile, meshFile);

	if (player != NULL)
		enemy->SetPlayerBullet(player->GetBullet());
}
