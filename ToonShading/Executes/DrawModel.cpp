#include "stdafx.h"
#include "DrawModel.h"

#include "../Units/GameSettings.h"

DrawModel::DrawModel(ExecuteValues* values)
	: Execute(values)
{
	settings = new GameSettings(values);
}

DrawModel::~DrawModel()
{
	SAFE_DELETE(settings);
}

void DrawModel::Update()
{
	settings->Update();
}

void DrawModel::ShadowRender()
{
}

void DrawModel::PreRender()
{
	settings->PreRender();
}

void DrawModel::LightRender()
{
	settings->LightRender();
}

void DrawModel::EdgeRender()
{
}

void DrawModel::AARender()
{
}


void DrawModel::ImGuiRender()
{
	settings->ImguiRender();
}