#include "stdafx.h"
#include "Tree.h"

Tree::Tree(wstring fileName)
{
	model = new Model();
	model->ReadMaterial(Models + L"Mesh/Quad.material");
	model->ReadMesh(Models + L"Mesh/Quad.mesh");
	
	for (Material* material : model->Materials())
		material->SetDiffuseMap(fileName);

	treeBuffer = new TreeBuffer();
	D3DXMatrixRotationY(&treeBuffer->Data.Rotate, (float)D3DX_PI);

	shader = new Shader(Shaders + L"999_Tree.hlsl");
	shader3 = new Shader(Shaders + L"999_Tree.hlsl", "VS", "PS_Depth");

	offset = power = 0.0f;
}

Tree::~Tree()
{
	SAFE_DELETE(model);
	SAFE_DELETE(treeBuffer);

	SAFE_DELETE(shader);
	SAFE_DELETE(shader3);
}

void Tree::AddTree(float scale, D3DXVECTOR3 Position)
{
	TreeStruct tree;
	tree.Position = Position;
	tree.Position.y += scale * 0.5f;
	tree.Scale = scale;

	D3DXMATRIX S, T;
	D3DXMatrixScaling(&S, tree.Scale, tree.Scale, 1.0f);
	D3DXMatrixTranslation(&T, tree.Position.x, tree.Position.y, tree.Position.z);
	tree.Transform = S * T;

	trees.push_back(tree);
}

void Tree::Update()
{
	offset += power * Time::Delta();					
	if (offset > D3DX_PI * 2) offset -= static_cast<float>(D3DX_PI * 2);	
	treeBuffer->Data.Power = sinf(offset);			

}

void Tree::PostRender2()
{
	treeBuffer->SetVSBuffer(2);

	for (Material* mat : model->Materials())
		mat->SetShader(shader3);

	for (TreeStruct tree : trees)
	{
		model->SetWorld(tree.Transform);
		for (ModelMesh* mesh : model->Meshes())
			mesh->Render();
	}
}

void Tree::Render()
{
	treeBuffer->SetVSBuffer(2);
	for (Material* mat : model->Materials())
		mat->SetShader(shader);

	for (TreeStruct tree : trees)
	{
		model->SetWorld(tree.Transform);
		for (ModelMesh* mesh : model->Meshes())
			mesh->Render();
	}
}
