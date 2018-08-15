#include "stdafx.h"
#include "MeshSphere.h"

MeshSphere::MeshSphere(ExecuteValues* values)
	: GameModel(Models + L"Meshes/Sphere.material", Models + L"Meshes/Sphere.mesh", values)
{
}

MeshSphere::~MeshSphere()
{
}

void MeshSphere::Update()
{
}

void MeshSphere::PreRender()
{
	__super::Update();
	__super::PreRender();
}

void MeshSphere::ImGuiRender()
{
}

void MeshSphere::SetShader(wstring shaderName)
{
	SAFE_DELETE(shader);
	shader = new Shader(shaderName);

	for (Material* material : model->Materials())
		material->SetShader(shader);
}
