#include "stdafx.h"
#include "MeshCube.h"

MeshCube::MeshCube(ExecuteValues* values)
	: GameModel(Models + L"Meshes/Cube.material", Models + L"Meshes/Cube.mesh", values)
{
}

MeshCube::~MeshCube()
{
}

void MeshCube::Update()
{
}

void MeshCube::PreRender()
{
}

void MeshCube::ImGuiRender()
{
}
