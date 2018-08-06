#pragma once

struct TreeStruct
{
	float Scale;
	D3DXVECTOR3 Position;
	D3DXMATRIX Transform;
};

class Tree
{
public:
public:
	Tree(wstring fileName);
	~Tree();

	void AddTree(float scale, D3DXVECTOR3 Position);

	void Update();

	void PostRender2();
	void Render();

	wstring FileName() { return fileName; }

	vector<TreeStruct> Trees() { return trees; }

	void TreeLoad(vector<TreeStruct> v);

private:
	Model* model;
	wstring fileName;

	Shader* shader;
	Shader* shader3;

	vector<TreeStruct> trees;

};