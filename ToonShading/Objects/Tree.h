#pragma once

struct TreeStruct
{
	float Scale;
	D3DXVECTOR3 Position;
	D3DXMATRIX Transform;

	bool Visible;
};

struct ExecuteValues;
class Tree
{
public:
	Tree(wstring fileName, ExecuteValues* values = NULL);
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