#include "stdafx.h"
#include "DrawModel.h"

#include "../Units/GameSettings.h"
#include "../Model/ModelAnimationPlayer.h"

DrawModel::DrawModel(ExecuteValues* values)
	: Execute(values)
{
	settings = new GameSettings(values);

	//model = new Model;
	//model->ReadMaterial(Models + L"Goblin/Goblin.material");
	//model->ReadMesh(Models + L"Goblin/Goblin.mesh");
	//model->ReadAnim(Models + L"Goblin/2Hit.anim", L"2Hit");
	//
	//player = new ModelAnimPlayer(model, L"2Hit");

}

DrawModel::~DrawModel()
{
	//SAFE_DELETE(player);
	//SAFE_DELETE(model);

	SAFE_DELETE(settings);
}

void DrawModel::Update()
{
	//player->Update();
	settings->Update();
}

void DrawModel::ShadowRender()
{
	//player->ShadowRender();
	settings->ShadowRender();
}

void DrawModel::PreRender()
{
	//player->PreRender();

	//model->Render();
	
	settings->PreRender();
}

void DrawModel::LightRender()
{
	settings->LightRender();
}

void DrawModel::SSAORender()
{
}

void DrawModel::EdgeRender()
{
}

void DrawModel::AARender()
{
}


void DrawModel::ImGuiRender()
{
	//player->ImGuiRender();
	settings->ImguiRender();
}