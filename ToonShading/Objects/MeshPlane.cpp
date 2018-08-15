#include "stdafx.h"
#include "MeshPlane.h"

MeshPlane::MeshPlane(ExecuteValues* values)
	: GameModel(Models + L"Meshes/Plane.material", Models + L"Meshes/Plane.mesh", values)
{
}

MeshPlane::~MeshPlane()
{
}

void MeshPlane::Update()
{
}

void MeshPlane::PreRender()
{
}

void MeshPlane::ImGuiRender()
{
}
