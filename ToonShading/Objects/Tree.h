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

	void DepthRender();
	void DiffuseRender();

	wstring FileName() { return fileName; }

	vector<TreeStruct> Trees() { return trees; }

	void TreeLoad(vector<TreeStruct> v);

private:
	Model* model;
	wstring fileName;

	Shader* diffuseShader;
	Shader* depthShader;

	vector<TreeStruct> trees;

};