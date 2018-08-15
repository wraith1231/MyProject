#include "stdafx.h"
#include "MeshCylinder.h"

MeshCylinder::MeshCylinder(ExecuteValues* values)
	: GameModel(Models + L"Meshes/Cylinder.material", Models + L"Meshes/Cylinder.mesh", values)
{
}

MeshCylinder::~MeshCylinder()
{
}

void MeshCylinder::Update()
{
}

void MeshCylinder::PreRender()
{
}

void MeshCylinder::ImGuiRender()
{
}
