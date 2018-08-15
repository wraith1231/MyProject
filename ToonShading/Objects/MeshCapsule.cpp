#include "stdafx.h"
#include "MeshCapsule.h"

MeshCapsule::MeshCapsule(ExecuteValues* values)
	: GameModel(Models + L"Meshes/Capsule.material", Models + L"Meshes/Capsule.mesh", values)
{
}

MeshCapsule::~MeshCapsule()
{
}

void MeshCapsule::Update()
{
}

void MeshCapsule::PreRender()
{
}

void MeshCapsule::ImGuiRender()
{
}
