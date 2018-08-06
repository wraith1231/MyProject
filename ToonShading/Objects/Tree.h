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
	class TreeBuffer : public ShaderBuffer
	{
	public:
		TreeBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			D3DXMatrixIdentity(&Data.Rotate);

			Data.WindDirection = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

			Data.Power = 0.0f;
		}

		struct Struct
		{
			D3DXMATRIX Rotate;

			D3DXVECTOR3 WindDirection;
			float Padding1;

			float Power;
			float Padding2[3];
		} Data;
	};

public:
	Tree(wstring fileName);
	~Tree();

	void AddTree(float scale, D3DXVECTOR3 Position);

	void Update();

	void PostRender2();
	void Render();

	wstring FileName() { return fileName; }

private:
	Model* model;
	wstring fileName;

	Shader* shader;
	Shader* shader3;

	TreeBuffer* treeBuffer;

	vector<TreeStruct> trees;

	float offset, power;
};