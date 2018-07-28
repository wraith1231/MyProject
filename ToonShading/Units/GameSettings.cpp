#include "stdafx.h"
#include "GameSettings.h"

#include "../Objects/GameSkyBox.h"
#include "../Objects/GameWorld.h"
#include "../Objects/GameTerrain.h"
#include "../Objects/GameUntouchable.h"

#include "../Bounding/ObjectsRay.h"

#include "GameEnemy.h"
#include "EnemyTank.h"

GamePlayer* GameSettings::player = NULL;
GameTerrain* GameSettings::terrain = NULL;

GameSettings::GameSettings()
	: skyBox(NULL), world(NULL)
	, enemy(NULL), model(NULL)
	, values(NULL)
	, inter(false), disposed(false)
	, untoTemp(NULL), untouchSel(false)
{
	CreateEnvironment();
	CreatePlayer();
	CreateEnemy();

}

GameSettings::~GameSettings()
{
	SAFE_DELETE(terrain);
	SAFE_DELETE(skyBox);
	SAFE_DELETE(world);

	SAFE_DELETE(enemy);
	SAFE_DELETE(player);

	SAFE_DELETE(model);
	for (GameAnimationModel* am : models)
		SAFE_DELETE(am);
}

void GameSettings::Update()
{
	if (values != NULL)
	{
		D3DXVECTOR3 cam, camDir;
		values->MainCamera->GetPosition(&cam);
		camDir = values->MainCamera->GetDirection(values->Viewport, values->Perspective);

		float dis = 0.0f;
		if (terrain != NULL)
			inter = terrain->Intersect(cam, camDir, dis);

		if (Mouse::Get()->Down(0) == true && inter == true)
		{
			if (untoTemp != NULL)
			{
				D3DXVECTOR3 pos = cam + camDir * dis;
			
				if (disposed == false)
				{
					disposed = true;
					untouchables.push_back(untoTemp);

				}
				untoTemp->AddTransforms(D3DXVECTOR3(1, 1, 1), D3DXVECTOR3(0, 0, 0), pos);
			}
			else if (untouchSel == true)
			{
				Objects::Ray* ray = new Objects::Ray(cam, camDir);

				bool b = false;
				for (GameUntouchable* un : untouchables)
				{
					if (b == true)
					{
						un->Unselect();
						continue;
					}

					if (un->SelectObject(ray) == true)
						b = true;
				}
			}
		}
	}

	for (GameUntouchable* unto : untouchables)
		unto->Update();

	if (skyBox != NULL)
		skyBox->Update();
	if (world != NULL)
		world->Update();
	if (terrain != NULL)
		terrain->Update();

	if (enemy != NULL)
		enemy->Update();
	if (player != NULL)
		player->Update();
	if (model != NULL)
		model->Update();
	for (GameAnimationModel* am : models)
		am->Update();
}

void GameSettings::Render()
{
	for (GameUntouchable* untouch : untouchables)
		untouch->Render();
	if (skyBox != NULL)
		skyBox->Render();
	if (world != NULL)
		world->Render();
	if (terrain != NULL)
		terrain->Render();

	if (enemy != NULL)
		enemy->Render();
	if (player != NULL)
		player->Render();
	if (model != NULL)
		model->Render();

	for (GameAnimationModel* am : models)
		am->Render();
}

void GameSettings::PreRender()
{
	for (GameUntouchable* untouch : untouchables)
		untouch->PreRender();
	if (skyBox != NULL)
		skyBox->PreRender();
	if (world != NULL)
		world->PreRender();
	if (terrain != NULL)
		terrain->PreRender();

	if (enemy != NULL)
		enemy->PreRender();
	if (player != NULL)
		player->PreRender();
	if (model != NULL)
		model->PreRender();
	for (GameAnimationModel* am : models)
		am->PreRender();
}

void GameSettings::PreRender2()
{
	for (GameUntouchable* untouch : untouchables)
		untouch->PreRender2();
	if (skyBox != NULL)
		skyBox->PreRender2();
	if (world != NULL)
		world->PreRender2();
	if (terrain != NULL)
		terrain->PreRender2();

	if (enemy != NULL)
		enemy->PreRender2();
	if (player != NULL)
		player->PreRender2();
	if (model != NULL)
		model->PreRender2();
	for (GameAnimationModel* am : models)
		am->PreRender2();
}

void GameSettings::PostRender()
{
}

void GameSettings::ImguiRender()
{
	ImGui::BeginMainMenuBar();

	{
		if (ImGui::BeginMenu("Model"))
		{
			if (ImGui::MenuItem("Untouchable Dispose"))
				DisposeUnTouchable();
			if (ImGui::MenuItem("Release Untouchable"))
			{
				if (untoTemp != NULL)
				{
					if (disposed == false)
						SAFE_DELETE(untoTemp);
					untoTemp = NULL;
				}

				disposed = false;
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Untouchable Select"))
			{
				if (untoTemp != NULL)
				{
					if (disposed == false)
						SAFE_DELETE(untoTemp);
					untoTemp = NULL;
				}

				disposed = false;

				untouchSel = !untouchSel;
			}

			ImGui::EndMenu();
		}
	}

	ImGui::EndMainMenuBar();

	ImGui::Begin("Untouchalbe Status");

	{
		if (untoTemp != NULL)
			ImGui::Text("Dispose Untouchable");

		if (untouchSel == true)
			ImGui::Text("Select Untouchable");
	}

	ImGui::End();

	for (GameUntouchable* un : untouchables)
		un->ImGuiRender();

	if (skyBox != NULL)
		skyBox->ImGuiRender();
	if (world != NULL)
		world->ImGuiRender();
	if (terrain != NULL)
		terrain->ImGuiRender();

	if (enemy != NULL)
		enemy->ImGuiRender();
	if (player != NULL)
		player->ImGuiRender();
	if (model != NULL)
		model->ImGuiRender();
	for (GameAnimationModel* am : models)
		am->ImGuiRender();

}

void GameSettings::DisposeUnTouchable(wstring file)
{
	D3DDesc descc;
	D3D::GetDesc(&descc);

	if (file.length() < 1)
	{
		function<void(wstring)> f;
		f = bind(&GameSettings::DisposeUnTouchable, this, placeholders::_1);

		Path::OpenFileDialog(L"", L"material or mesh\0", Models, f, descc.Handle);

		return;
	}

	if (untoTemp != NULL)
	{
		if (disposed == false)
			SAFE_DELETE(untoTemp);
		untoTemp = NULL;
	}

	disposed = false;
	untoTemp = new GameUntouchable(file);

	return;
}

void GameSettings::CreateEnvironment()
{
	//wstring materialFile = Models + L"Grund/Grund.material";
	//wstring meshFile = Models + L"Grund/Grund.mesh";
	//wstring animPath = Models + L"Grund/";
	//
	//materialFile = Models + L"Stage/france.material";
	//meshFile = Models + L"Stage/france.mesh";
	//world = new GameWorld(materialFile, meshFile);
	//
	//materialFile = Models + L"Stage/france_sky.material";
	//meshFile = Models + L"Stage/france_sky.mesh";
	//skyBox = new GameSkyBox(materialFile, meshFile);
	//
	//skyBox->SetBasisPosition({ 0, 0, 0 });

	terrain = new GameTerrain();
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
