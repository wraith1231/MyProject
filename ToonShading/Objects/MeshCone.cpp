#include "stdafx.h"
#include "MeshCone.h"

MeshCone::MeshCone(ExecuteValues* values)
	: GameModel(Models + L"Meshes/Cone.material", Models + L"Meshes/Cone.mesh", values)
{
}

MeshCone::~MeshCone()
{
}

void MeshCone::Update()
{
}

void MeshCone::PreRender()
{
	__super::Update();
	__super::PreRender();
}

void MeshCone::ImGuiRender()
{
}
