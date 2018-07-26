#include "stdafx.h"
#include "DrawModel.h"

#include "../Units/GameSettings.h"

DrawModel::DrawModel(ExecuteValues* values)
	: Execute(values)
{
	settings = new GameSettings();
}

DrawModel::~DrawModel()
{
	SAFE_DELETE(settings);
}

void DrawModel::Update()
{
	settings->Update();
}

void DrawModel::PreRender()
{
	settings->PreRender();
}

void DrawModel::PreRender2()
{
	settings->PreRender2();
}

void DrawModel::Render()
{
	settings->Render();
}

void DrawModel::PostRender()
{
	settings->PostRender();
}

void DrawModel::PostRender2()
{
}

void DrawModel::ImGuiRender()
{
	settings->ImguiRender();
}