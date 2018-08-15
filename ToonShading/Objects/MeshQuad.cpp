#include "stdafx.h"
#include "MeshQuad.h"

MeshQuad::MeshQuad(ExecuteValues* values)
	: GameModel(Models + L"Meshes/Quad.material", Models + L"Meshes/Quad.mesh", values)
{
}

MeshQuad::~MeshQuad()
{
}

void MeshQuad::Update()
{
}

void MeshQuad::PreRender()
{
}

void MeshQuad::ImGuiRender()
{
}
