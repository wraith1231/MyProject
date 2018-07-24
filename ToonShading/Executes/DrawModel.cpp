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
����
�κ� ���� ���� ���µ� ã���� NULL�� �Ǿ��ִ� �ѱ� ����Ʈ�� ����
���� ��(��� �ΰ�) ã�Ƽ� �ش� �� ��ġ���� sphere�� �������� �߻�
���� �浹�ڽ��� ���� �ִµ� sphere�� �ڽ��� �浹 ����
+ �ϳ��� �Ѿ��� ����, �ϳ��� ����� 
+ ���� �̵� ����Ʈ�� � �����ؼ� catmull rom ������� �ڿ������� �̵�
ĳ���͵� �װ� ���󰡰�
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
