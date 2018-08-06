#include "stdafx.h"
#include "Tree.h"

Tree::Tree(wstring fileName)
	: fileName(fileName)
{
	model = new Model();
	model->ReadMaterial(Models + L"Mesh/Quad.material");
	model->ReadMesh(Models + L"Mesh/Quad.mesh");
	
	for (Material* material : model->Materials())
		material->SetDiffuseMap(fileName);


	shader = new Shader(Shaders + L"999_Tree.hlsl");
	shader3 = new Shader(Shaders + L"999_Tree.hlsl", "VS", "PS_Depth");
}

Tree::~Tree()
{
	SAFE_DELETE(model);

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

}

void Tree::PostRender2()
{

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
	for (Material* mat : model->Materials())
		mat->SetShader(shader);

	for (TreeStruct tree : trees)
	{
		model->SetWorld(tree.Transform);
		for (ModelMesh* mesh : model->Meshes())
			mesh->Render();
	}
}

void Tree::TreeLoad(vector<TreeStruct> v)
{
	trees.assign(v.begin(), v.end());
}
