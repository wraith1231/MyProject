#include "stdafx.h"
#include "DrawTerrain.h"

DrawTerrain::DrawTerrain(ExecuteValues* values)
	: Execute(values)
{
	terrain = new Terrain(values);

	terrain->SetTexture(0, Landscapes + L"Dirt.png");
	terrain->SetTexture(1, Landscapes + L"Dirt2.png");
	terrain->SetTexture(2, Landscapes + L"Dirt3.png");
}

DrawTerrain::~DrawTerrain()
{
	SAFE_DELETE(terrain);
}

void DrawTerrain::Update()
{
	if (Keyboard::Get()->Down('P'))
		terrain->SetHeightMap(Landscapes + L"HeightMap256.png");

	terrain->Update();
}

void DrawTerrain::PreRender()
{

}

void DrawTerrain::Render()
{
	terrain->Render();
}

void DrawTerrain::PostRender()
{
	terrain->PostRender();
}