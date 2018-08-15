#include "stdafx.h"
#include "Tree.h"

Tree::Tree(wstring fileName, ExecuteValues* values)
	: fileName(fileName)
{
	model = new Model();
	model->ReadMaterial(Models + L"Mesh/Quad.material");
	model->ReadMesh(Models + L"Mesh/Quad.mesh");
	model->SetExecuteValue(values);
	
	for (Material* material : model->Materials())
		material->SetDiffuseMap(fileName);


	diffuseShader = new Shader(Shaders + L"999_Tree.hlsl");
	depthShader = new Shader(Shaders + L"999_Tree.hlsl", "VS", "PS_Depth");
	normalShader = new Shader(Shaders + L"999_Tree.hlsl", "VS", "PS_Normal");
}

Tree::~Tree()
{
	SAFE_DELETE(model);

	SAFE_DELETE(normalShader);
	SAFE_DELETE(diffuseShader);
	SAFE_DELETE(depthShader);
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
	tree.Visible = true;

	trees.push_back(tree);
}

void Tree::Update()
{
}

void Tree::NormalRender()
{
	for (Material* mat : model->Materials())
		mat->SetShader(normalShader);

	for (TreeStruct tree : trees)
	{
		model->SetWorld(tree.Transform);
		model->VisibleUpdate();
		tree.Visible = model->GetVisible();

		if (tree.Visible == false)
			continue;

		for (ModelMesh* mesh : model->Meshes())
			mesh->Render();
	}
}

void Tree::DepthRender()
{
	for (Material* mat : model->Materials())
		mat->SetShader(depthShader);


	for (TreeStruct tree : trees)
	{
		model->SetWorld(tree.Transform);
		if (tree.Visible == false)
			continue;

		for (ModelMesh* mesh : model->Meshes())
			mesh->Render();
	}
}

void Tree::DiffuseRender()
{
	for (Material* mat : model->Materials())
		mat->SetShader(diffuseShader);

	for (TreeStruct tree : trees)
	{
		if (tree.Visible == false)
			continue;

		model->SetWorld(tree.Transform);
		for (ModelMesh* mesh : model->Meshes())
			mesh->Render();
	}
}

void Tree::TreeLoad(vector<TreeStruct> v)
{
	trees.assign(v.begin(), v.end());
}
