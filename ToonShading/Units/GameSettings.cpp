#include "stdafx.h"
#include "GameSettings.h"

#include <fstream>

#include "../Objects/GameSkyBox.h"
#include "../Objects/GameWorld.h"
#include "../Objects/GameTerrain.h"
#include "../Objects/GameUntouchable.h"

#include "../Bounding/ObjectsRay.h"
#include "../Bounding/BoundingBox.h"

#include "GameEnemy.h"
#include "EnemyTank.h"
#include "Bullet.h"

GamePlayer* GameSettings::player = NULL;
GameTerrain* GameSettings::terrain = NULL;
vector<GameUntouchable*> GameSettings::untouchables = vector<GameUntouchable*>();

GameSettings::GameSettings(ExecuteValues* values)
	: skyBox(NULL), world(NULL)
	, enemy(NULL), model(NULL)
	, inter(false), disposed(false)
	, untoTemp(NULL), untouchSel(false)
	, enemyDispose(false), playerDispose(false)
	, editMode(false), enemySel(false)
	, sceneFile(L"")
	, values(values)
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
	if (values != NULL && editMode == true)
	{
		values->MainCamera->SetTarget(NULL);
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
				//untouchable dispose
				D3DXVECTOR3 pos = cam + camDir * dis;
				Objects::Ray* ray = new Objects::Ray(cam, camDir);
			
				bool b = false;
				for (GameUntouchable* un : untouchables)
				{
					if (un->ObjectCheck(ray) == true)
					{
						b = true;
						break;
					}
				}
				if (b == false)
				{
					for (GameEnemy* en : enemies)
					{
						if (en->Box()->Intersects(ray) == true)
						{
							b = true;
							break;
						}
					}
				}
				if (b == false)
				{
					if (player->Box()->Intersects(ray) == true)
					{
						b = true;
					}
				}

				SAFE_DELETE(ray);

				if (b == false)
				{

					if (disposed == false)
					{
						disposed = true;
						untouchables.push_back(untoTemp);

					}
					untoTemp->AddTransforms(D3DXVECTOR3(1, 1, 1), D3DXVECTOR3(0, 0, 0), pos);
				}
			}
			else if (untouchSel == true)
			{
				//untouchable select
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
				SAFE_DELETE(ray);
			}
			else if (enemyDispose == true)
			{
				Objects::Ray* ray = new Objects::Ray(cam, camDir);
				bool b = false;
				for (GameUntouchable* un : untouchables)
				{
					if (un->ObjectCheck(ray) == true)
					{
						b = true;
						break;
					}
				}
				if (b == false)
				{
					for (GameEnemy* en : enemies)
					{
						if (en->Box()->Intersects(ray) == true)
						{
							b = true;
							break;
						}
					}
				}
				if (b == false)
				{
					if (player->Box()->Intersects(ray) == true)
					{
						b = true;
					}
				}

				SAFE_DELETE(ray);

				if(b == false)
					DisposeEnemy(cam + camDir * dis);
			}
			else if (playerDispose == true)
			{
				Objects::Ray* ray = new Objects::Ray(cam, camDir);
				bool b = false;
				for (GameUntouchable* un : untouchables)
				{
					if (un->ObjectCheck(ray) == true)
					{
						b = true;
						break;
					}
				}
				if (b == false)
				{
					for (GameEnemy* en : enemies)
					{
						if (en->Box()->Intersects(ray) == true)
						{
							b = true;
							break;
						}
					}
				}
				if (b == false)
				{
					if (player->Box()->Intersects(ray) == true)
					{
						b = true;
					}
				}

				SAFE_DELETE(ray);

				if (b == false)
					player->StartPoint(cam + camDir * dis);
			}
			else if (enemySel == true)
			{
				//enemy select
				Objects::Ray* ray = new Objects::Ray(cam, camDir);

				bool b = false;
				for (GameEnemy* en : enemies)
				{
					if (b == true)
					{
						en->Unselect();
						continue;
					}

					if (en->SelectEnemy(ray) == true)
						b = true;
				}
				SAFE_DELETE(ray);
			}
		}
		else if (Mouse::Get()->Press(0) && inter == true)
		{
			if (terrain->EditMode() == true)
				terrain->EditTerrain();
		}
		for (GameUntouchable* unto : untouchables)
			unto->Update();
		if (terrain != NULL)
			terrain->Update();
		if (skyBox != NULL)
			skyBox->Update();
		if (world != NULL)
			world->Update();

		for (GameEnemy* en : enemies)
			en->EditUpdate();
		if (player != NULL)
			player->EditUpdate();
	}
	else if (editMode == false)
	{
		
		for (GameUntouchable* unto : untouchables)
			unto->Update();
		if (terrain != NULL)
			terrain->Update();
		if (skyBox != NULL)
			skyBox->Update();
		if (world != NULL)
			world->Update();

		if (enemy != NULL)
			enemy->Update();

		for (GameEnemy* ene : enemies)
			ene->Update();
		if (player != NULL)
		{
			player->SetCamTarget();
			player->Update();
		}
		if (model != NULL)
			model->Update();
		for (GameAnimationModel* am : models)
			am->Update();

		for (GameEnemy* ene : enemies)
		{
			UINT num = 0;
			
			if (playerBullet->CheckIntersect(ene->Box(), num) == true)
			{
				playerBullet->DeleteBullet(num);
			}
		}
		for (Bullet* bullet : enemyBullets)
		{
			UINT num = 0;
			if (bullet->CheckIntersect(player->Box(), num) == true)
			{
				bullet->DeleteBullet(num);
			}
		}

		for (GameUntouchable* un : untouchables)
		{
			UINT num = 0;
			for (UINT i = 0; i < un->Transforms(); i++)
			{
				if (playerBullet->CheckIntersect(un->Box(i), num) == true)
				{
					playerBullet->DeleteBullet(num);
				}

				for (Bullet* bullet : enemyBullets)
				{
					if (bullet->CheckIntersect(un->Box(i), num) == true)
					{
						bullet->DeleteBullet(num);
					}
				}
			}
		}
	}
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
	for (GameEnemy* ene : enemies)
		ene->Render();
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
	for (GameEnemy* ene : enemies)
		ene->PreRender();
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
	for (GameEnemy* ene : enemies)
		ene->PreRender2();
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
	if (editMode == true)
	{

		ImGui::BeginMainMenuBar();

		//Scene
		{
			if (ImGui::BeginMenu("Scene"))
			{
				if (ImGui::MenuItem("Scene Save"))
				{
					untouchSel = false;
					playerDispose = false;
					enemyDispose = false;
					enemySel = false;
					terrain->EditMode(false);
					for (GameUntouchable* un : untouchables)
						un->Unselect();
					SaveScene();
				}
				if (ImGui::MenuItem("Scene Load"))
				{
					untouchSel = false;
					playerDispose = false;
					enemyDispose = false;
					enemySel = false;
					terrain->EditMode(false);
					for (GameUntouchable* un : untouchables)
						un->Unselect();
					LoadScene();
				}

				ImGui::EndMenu();
			}
		}
		//Model
		{
			if (ImGui::BeginMenu("Model"))
			{
				if (ImGui::MenuItem("Untouchable Dispose"))
				{
					untouchSel = false;
					playerDispose = false;
					enemyDispose = false;
					enemySel = false;
					terrain->EditMode(false);
					for (GameUntouchable* un : untouchables)
						un->Unselect();

					DisposeUnTouchable();
				}
				if (ImGui::MenuItem("Release Untouchable"))
				{
					if (untoTemp != NULL)
					{
						if (disposed == false)
							SAFE_DELETE(untoTemp);
						untoTemp = NULL;
					}
					for (GameUntouchable* un : untouchables)
						un->Unselect();

					disposed = false;

					untouchSel = false;
					playerDispose = false;
					enemyDispose = false;
					enemySel = false;
					terrain->EditMode(false);
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
					for (GameUntouchable* un : untouchables)
						un->Unselect();

					disposed = false;

					untouchSel = !untouchSel;
					playerDispose = false;
					enemyDispose = false;
					enemySel = false;
					terrain->EditMode(false);
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Tank Dispose"))
				{
					if (untoTemp != NULL)
					{
						if (disposed == false)
							SAFE_DELETE(untoTemp);
						untoTemp = NULL;
					}
					for (GameUntouchable* un : untouchables)
						un->Unselect();

					disposed = false;

					untouchSel = false;
					playerDispose = false;
					enemySel = false;
					terrain->EditMode(false);
					enemyDispose = !enemyDispose;
				}

				if (ImGui::MenuItem("Enemy Select"))
				{

					if (untoTemp != NULL)
					{
						if (disposed == false)
							SAFE_DELETE(untoTemp);
						untoTemp = NULL;
					}
					for (GameUntouchable* un : untouchables)
						un->Unselect();

					disposed = false;

					untouchSel = false;
					terrain->EditMode(false);
					playerDispose = false;
					enemySel = !enemySel;
					enemyDispose = false;
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Player Dispose"))
				{
					if (untoTemp != NULL)
					{
						if (disposed == false)
							SAFE_DELETE(untoTemp);
						untoTemp = NULL;
					}
					for (GameUntouchable* un : untouchables)
						un->Unselect();

					disposed = false;

					untouchSel = false;
					enemyDispose = false;
					terrain->EditMode(false);
					enemySel = false;
					playerDispose = !playerDispose;
				}

				ImGui::EndMenu();
			}
		}

		ImGui::EndMainMenuBar();
	}

	ImGui::Begin("Edit Mode Activate/Deactivate");
	bool b = editMode;
	ImGui::Checkbox("Edit Mode", &editMode);
	if (b != editMode && editMode == false)
	{
		if (untoTemp != NULL)
		{
			if (disposed == false)
				SAFE_DELETE(untoTemp);
			untoTemp = NULL;
		}
		for (GameUntouchable* un : untouchables)
			un->Unselect();

		disposed = false;

		untouchSel = false;
		enemyDispose = false;
		terrain->EditMode(false);
		enemySel = false;
		playerDispose = false;
	}

	ImGui::End();

	ImGui::Begin("Untouchalbe Status");

	{
		if (untoTemp != NULL)
			ImGui::Text("Dispose Untouchable");

		if (untouchSel == true)
			ImGui::Text("Select Untouchable");

		if (enemyDispose == true)
			ImGui::Text("Dispose Enemy");

		if (playerDispose == true)
		{
			ImGui::Text("Dispose Player Start Position");
			ImGui::Text("Player Start Pos : %.3f, %.3f, %.3f", player->StartPoint().x, player->StartPoint().y, player->StartPoint().z);
		}

		if (enemySel == true)
		{
			ImGui::Text("Enemy Select");
		}
	}

	ImGui::End();

	if (editMode == true)
	{
		for (GameUntouchable* un : untouchables)
			un->ImGuiRender();

		if (skyBox != NULL)
			skyBox->ImGuiRender();
		if (world != NULL)
			world->ImGuiRender();
		if (terrain != NULL)
			terrain->ImGuiRender();

		for (GameEnemy* en : enemies)
			en->ImGuiRender();
		if (enemy != NULL)
			enemy->ImGuiRender();
		if (player != NULL)
			player->ImGuiRender();
		if (model != NULL)
			model->ImGuiRender();
		for (GameAnimationModel* am : models)
			am->ImGuiRender();
	}
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


	wstring local = Path::GetFileLocalPathDirectory(file);
	wstring name = Path::GetFileNameWithoutExtension(file);
	local = local + name;

	for (GameUntouchable* un : untouchables)
	{
		if (local == un->GetFileName())
		{
			disposed = true;
			untoTemp = un;
			return;
		}

	}
	disposed = false;

	untoTemp = new GameUntouchable(file);

	return;
}

void GameSettings::SaveScene()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);
	MessageBox(desc.Handle, L"Terrain Save", L"Save", MB_OK);
	terrain->SaveTerrain();

	if (terrain->GetFileName().length() < 1) return;

	Json::Value* root = new Json::Value;

	//terrain
	Json::Value ter;
	string s = String::ToString(terrain->GetFileName());
	Json::SetValue(ter, "Terrain Save", s);
	(*root)["Terrain"] = ter;

	//untouchable
	Json::Value* untou = new Json::Value;
	Json::Value untou1;
	int temp = untouchables.size();
	Json::SetValue(untou1, "Untouchable Quantity", temp);
	(*untou)["Untouchables"] = untou1;
	for (UINT i = 0; i < untouchables.size(); i++)
	{
		untouchables[i]->SaveTransforms(untou, i);
	}
	//for (GameUntouchable* un : untouchables)
	//{
	//	un->SaveTransforms(untou);
	//}
	(*root)["Untouchable"] = *untou;

	//enemy
	Json::Value enem;
	int enemSize = enemies.size();
	Json::SetValue(enem, "Enemy Quantity", enemSize);
	for (UINT i = 0; i < enemies.size(); i++)
	{
		Json::Value val;
		Json::SetValue(val, "Start Position", enemies[i]->StartPoint());
		string s = "Enemy" + to_string(i);
		enem[s] = val;
	}
	(*root)["Enemy"] = enem;

	//player
	Json::Value pla;
	Json::SetValue(pla, "Player Position", player->StartPoint());
	(*root)["Player"] = pla;

	//Scene Save
	MessageBox(desc.Handle, L"Scene Save", L"Save", MB_OK);
	
	SceneSave();

	if (sceneFile.length() < 1)
	{
		SAFE_DELETE(root);
		SAFE_DELETE(untou);

		return;
	}

	ofstream stream;

	if (Path::GetExtension(sceneFile) != L"json")
		sceneFile += L".json";

	stream.open(sceneFile);
	{
		Json::StyledWriter writer;

		stream << writer.write(*root);
		//C4996을 옵션에서 막을 수도 있고 미리 컴파일된 헤더에서 막을 수도 있음
		//회사에선 warning 막으면 퇴사 사유니까 절-대로 하지 마라
	}
	stream.close();

	SAFE_DELETE(untou);
	SAFE_DELETE(root);
}

void GameSettings::LoadScene()
{
	DeleteAll();

	SceneLoad();

	if (sceneFile.length() < 1) return;

	Json::Value* root = new Json::Value;

	ifstream stream;

	wstring file = Path::GetFileLocalPath(sceneFile);

	stream.open(file);
	{
		Json::Reader reader;
		reader.parse(stream, *root);
	}
	stream.close();

	//Terrain
	Json::Value ter = (*root)["Terrain"];
	string s = "";
	Json::GetValue(ter, "Terrain Save", s);

	terrain->LoadTerrain(String::ToWString(s));

	//Untouchable
	Json::Value* untou = &(*root)["Untouchable"];

	Json::Value untou1 = (*untou)["Untouchables"];
	int quan;
	Json::GetValue(untou1, "Untouchable Quantity", quan);

	for (int i = 0; i < quan; i++)
	{
		s = "Untouchable" + to_string(i);
		Json::Value* tem = &(*untou)[s];

		Json::Value va = (*tem)["Structs"];
	
		Json::GetValue(va, "File Name", s);
		untoTemp = new GameUntouchable(String::ToWString(s));

		int qu;
		Json::GetValue(va, "Quantity", qu);

		for (int j = 0; j < qu; j++)
		{
			s = "Number" + to_string(j);
			Json::Value v = va[s];
			D3DXVECTOR3 scale, rotate, trans;
			Json::GetValue(v, "Scale", scale);
			Json::GetValue(v, "Rotate", rotate);
			Json::GetValue(v, "Trans", trans);

			untoTemp->AddTransforms(scale, rotate, trans);
		}

		untouchables.push_back(untoTemp);
		untoTemp = NULL;
	}

	//Enemy
	Json::Value enem = (*root)["Enemy"];
	int size;
	Json::GetValue(enem, "Enemy Quantity", size);
	for (int i = 0; i < size; i++)
	{
		s = "Enemy" + to_string(i);
		Json::Value val = enem[s];
		D3DXVECTOR3 pos;
		Json::GetValue(val, "Start Position", pos);
		DisposeEnemy(pos);
	}

	Json::Value pla = (*root)["Player"];
	D3DXVECTOR3 pos;
	Json::GetValue(pla, "Player Position", pos);
	player->StartPoint(pos);
	player->Position(pos);

	//SAFE_DELETE(untou);
	SAFE_DELETE(root);
}

void GameSettings::SceneSave(wstring file)
{
	D3DDesc descc;
	D3D::GetDesc(&descc);

	if (file.length() < 1)
	{
		function<void(wstring)> f;
		f = bind(&GameSettings::SceneSave, this, placeholders::_1);

		Path::OpenFileDialog(L"", Path::JsonFilter, Jsons, f, descc.Handle);

		return;
	}

	sceneFile = file;
	return;
}

void GameSettings::SceneLoad(wstring file)
{
	D3DDesc descc;
	D3D::GetDesc(&descc);

	if (file.length() < 1)
	{
		function<void(wstring)> f;
		f = bind(&GameSettings::SceneLoad, this, placeholders::_1);

		Path::OpenFileDialog(L"", Path::JsonFilter, Jsons, f, descc.Handle);

		return;
	}

	sceneFile = file;
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
	player = new GamePlayer(materialFile, meshFile, animPath, values);

	playerBullet = player->GetBullet();
}

void GameSettings::CreateEnemy()
{
	//wstring matFile = Models + L"Enemies/Tiger.material";
	//wstring meshFile = Models + L"Enemies/Tiger.mesh";
	//enemy = new EnemyTank(matFile, meshFile);
	//
	//enemy->StartAi(GameEnemy::AiType::Move, 5.0f);
	//enemy->SetPosition(D3DXVECTOR3(0, 0, 30));
	//D3DXMATRIX R;
	//D3DXMatrixRotationY(&R, Math::ToRadian(180));
	//enemy->RootAxis(R);
}

void GameSettings::DisposeEnemy(D3DXVECTOR3 position)
{
	wstring matFile = Models + L"Enemies/Tiger.material";
	wstring meshFile = Models + L"Enemies/Tiger.mesh";
	GameEnemy* ene = new EnemyTank(matFile, meshFile);
	
	ene->StartAi(GameEnemy::AiType::Move, Math::Random(0.0f, 1.0f));
	ene->StartPoint(position);
	ene->SetPosition(position);
	D3DXMATRIX R;
	D3DXMatrixRotationY(&R, Math::ToRadian(180));
	ene->RootAxis(R);
	enemyBullets.push_back(ene->GetBullet());
	ene->Update();

	enemies.push_back(ene);
}

void GameSettings::DeleteAll()
{
	for (GameUntouchable* un : untouchables)
		SAFE_DELETE(un);
	untouchables.clear();

	for (GameEnemy* en : enemies)
		SAFE_DELETE(en);
	enemies.clear();

	player->BulletClear();
}
