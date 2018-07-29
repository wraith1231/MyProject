#include "stdafx.h"
#include "GameUntouchable.h"

#include "../Bounding/BoundingBox.h"

GameUntouchable::GameUntouchable(wstring file)
{
	model = new Model;
	wstring local = Path::GetFileLocalPathDirectory(file);
	wstring name = Path::GetFileNameWithoutExtension(file);
	local = local + name;
	this->file = local;
	model->ReadMaterial(local + L".material");
	model->ReadMesh(local + L".mesh");

	shader = new Shader(Shaders + L"999_Mesh.hlsl");
	shader2 = new Shader(Shaders + L"999_Mesh.hlsl", "VS_Normal", "PS_Normal");
	shader3 = new Shader(Shaders + L"999_Mesh.hlsl", "VS_Depth", "PS_Depth");

	D3DXVECTOR3 max, min;
	max = D3DXVECTOR3(-9999, -9999, -9999);
	min = -max;
	model->CheckMaxMinVer(max, min);

	box = new Objects::BoundingBox(max, min);
	selected = false;
	selectNum = 0;
	setMode = 0;
}

GameUntouchable::~GameUntouchable()
{
	SAFE_DELETE(model);

	SAFE_DELETE(shader);
	SAFE_DELETE(shader2);
	SAFE_DELETE(shader3);

	SAFE_DELETE(box);

	transforms.clear();
}

void GameUntouchable::AddTransforms(D3DXVECTOR3 scale, D3DXVECTOR3 rotate, D3DXVECTOR3 trans)
{
	ModelStruct* mod = new ModelStruct;
	mod->Scale = scale;
	mod->Rotate = rotate;
	mod->Translation = trans;

	transforms.push_back(mod);
}

bool GameUntouchable::SelectObject(Objects::Ray * ray)
{
	selected = false;
	selectNum = 0;

	for (size_t i = 0; i < transforms.size(); i++)
	{
		box->Update(transforms[i]->Transforms);

		if (box->Intersects(ray) == true)
		{
			selected = true;
			selectNum = i;
			return true;
		}
	}

	return false;
}

bool GameUntouchable::ObjectCheck(Objects::Ray * ray)
{
	for (size_t i = 0; i < transforms.size(); i++)
	{
		box->Update(transforms[i]->Transforms);

		if (box->Intersects(ray) == true)
		{
			return true;
		}
	}

	return false;
}

void GameUntouchable::Update()
{
	if (selected == true)
	{
		SelectObjectUpdate();

	}

	for (ModelStruct* mod : transforms)
	{
		D3DXMATRIX S, R;
		D3DXMatrixScaling(&S, mod->Scale.x, mod->Scale.y, mod->Scale.z);
		D3DXMatrixRotationYawPitchRoll(&R, mod->Rotate.y, mod->Rotate.x, mod->Rotate.z);
		//D3DXMatrixRotationQuaternion(&R, &mod->Rotate);
		R._41 = mod->Translation.x;
		R._42 = mod->Translation.y;
		R._43 = mod->Translation.z;

		mod->Transforms = S * R;
	}
}

void GameUntouchable::SelectObjectUpdate()
{
	//0 = scale, 1 = rotate, 2 = translation
	if (Keyboard::Get()->Down('P'))
	{
		setMode = (setMode + 1) % 3;
	}

	float delta = Time::Delta();

	//Y+
	if (Keyboard::Get()->Press('I'))
	{
		switch (setMode)
		{
		case 0:
			transforms[selectNum]->Scale.y += 1.0f * delta;
			break;
		case 1:
			transforms[selectNum]->Rotate.y += 1.0f * delta;
			break;
		case 2:
			transforms[selectNum]->Translation.y += 1.0f * delta;
			break;
		}
	}
	//Y-
	if (Keyboard::Get()->Press('K'))
	{
		switch (setMode)
		{
		case 0:
			transforms[selectNum]->Scale.y -= 1.0f * delta;
			break;
		case 1:
			transforms[selectNum]->Rotate.y -= 1.0f * delta;
			break;
		case 2:
			transforms[selectNum]->Translation.y -= 1.0f * delta;
			break;
		}
	}

	//X+
	if (Keyboard::Get()->Press('J'))
	{
		switch (setMode)
		{
		case 0:
			transforms[selectNum]->Scale.x += 1.0f * delta;
			break;
		case 1:
			transforms[selectNum]->Rotate.x += 1.0f * delta;
			break;
		case 2:
			transforms[selectNum]->Translation.x += 1.0f * delta;
			break;
		}
	}
	//X-
	if (Keyboard::Get()->Press('L'))
	{
		switch (setMode)
		{
		case 0:
			transforms[selectNum]->Scale.x -= 1.0f * delta;
			break;
		case 1:
			transforms[selectNum]->Rotate.x -= 1.0f * delta;
			break;
		case 2:
			transforms[selectNum]->Translation.x -= 1.0f * delta;
			break;
		}
	}

	//Z+
	if (Keyboard::Get()->Press('U'))
	{
		switch (setMode)
		{
		case 0:
			transforms[selectNum]->Scale.z += 1.0f * delta;
			break;
		case 1:
			transforms[selectNum]->Rotate.z += 1.0f * delta;
			break;
		case 2:
			transforms[selectNum]->Translation.z += 1.0f * delta;
			break;
		}
	}
	//Z-
	if (Keyboard::Get()->Press('O'))
	{
		switch (setMode)
		{
		case 0:
			transforms[selectNum]->Scale.z -= 1.0f * delta;
			break;
		case 1:
			transforms[selectNum]->Rotate.z -= 1.0f * delta;
			break;
		case 2:
			transforms[selectNum]->Translation.z -= 1.0f * delta;
			break;
		}
	}
}

void GameUntouchable::Unselect()
{
	selected = false;
	selectNum = 0;
	setMode = 0;
}


void GameUntouchable::PreRender()
{
	for (Material* material : model->Materials())
		material->SetShader(shader2);

	for (ModelStruct* mod : transforms)
	{
		for (ModelMesh* mesh : model->Meshes())
		{
			mesh->SetWorld(mod->Transforms);
			mesh->Render();
		}
	}
}

void GameUntouchable::PreRender2()
{
	for (Material* material : model->Materials())
		material->SetShader(shader3);

	for (ModelStruct* mod : transforms)
	{
		for (ModelMesh* mesh : model->Meshes())
		{
			mesh->SetWorld(mod->Transforms);
			mesh->Render();
		}
	}
}

void GameUntouchable::Render()
{
	for (Material* material : model->Materials())
		material->SetShader(shader);

	for (ModelStruct* mod : transforms)
	{
		for (ModelMesh* mesh : model->Meshes())
		{
			mesh->SetWorld(mod->Transforms);
			mesh->Render();
		}
	}
}

void GameUntouchable::ImGuiRender()
{
	if (selected == true)
	{
		ImGui::Begin("Selected Untouchable");

		ImGui::Text(String::ToString(file).c_str());
		if (setMode == 0)
			ImGui::Text("Now Scale Change");
		if (setMode == 1)
			ImGui::Text("Now Rotate Change");
		if (setMode == 2)
			ImGui::Text("Now Translation Change");

		ImGui::Text("I = Y+, K = Y-, J = X+, L = X-, U = Z+, O = Z-");
		ImGui::Text("P = Mode Change");

		ImGui::InputFloat3("Scale", transforms[selectNum]->Scale);
		ImGui::InputFloat3("Rotate", transforms[selectNum]->Rotate);
		ImGui::InputFloat3("Translation", transforms[selectNum]->Translation);

		ImGui::End();
	}

}

void GameUntouchable::SaveTransforms(Json::Value* val, UINT i)
{
	Json::Value* tem = new Json::Value;
	Json::Value va;
	Json::SetValue(va, "File Name", String::ToString(file));
	int temp = transforms.size();
	Json::SetValue(va, "Quantity", temp);

	for (UINT i = 0; i < transforms.size(); i++)
	{
		Json::Value v;
		Json::SetValue(v, "Scale", transforms[i]->Scale);
		Json::SetValue(v, "Rotate", transforms[i]->Rotate);
		Json::SetValue(v, "Trans", transforms[i]->Translation);

		string s = "Number" + to_string(i);
		va[s] = v;
	}

	(*tem)["Structs"] = va;

	string s = "Untouchable" + to_string(i);
	(*val)[s] = *tem;
}

UINT GameUntouchable::Transforms()
{
	return transforms.size();
}

Objects::BoundingBox * GameUntouchable::Box(UINT num)
{
	box->Update(transforms[num]->Transforms);

	return box;
}
