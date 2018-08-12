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

void DrawModel::NormalRender()
{
	settings->NormalRender();
}

void DrawModel::DepthRender()
{
	settings->DepthRender();
}

void DrawModel::DiffuseRender()
{
	settings->DiffuseRender();
}

void DrawModel::LightRender()
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
	settings->ImguiRender();
}