#pragma once

struct QuadStruct
{
	D3DXVECTOR3 LB;	//Left Bottom
	D3DXVECTOR3 RB; //Right Bottom
	D3DXVECTOR3 LT; //Left Top
	D3DXVECTOR3 RT; // Right Top

	QuadStruct()
	{
		LB = D3DXVECTOR3(0, 0, 0);
		RB = D3DXVECTOR3(0, 0, 0);
		LT = D3DXVECTOR3(0, 0, 0);
		RT = D3DXVECTOR3(0, 0, 0);
	}
};

class GameTerrain
{
public:
	class TerrainBuffer : public ShaderBuffer
	{
	public:
		TerrainBuffer() : ShaderBuffer(&Data, sizeof(Struct))
		{
			Data.On = 0;
			Data.Type = 0;
			Data.Distance = 5.0f;
			Data.Point = D3DXVECTOR3(-1, -1, -1);
		}

		struct Struct
		{
			UINT On;
			UINT Type;
			float Distance;
			float Padding1;
			D3DXVECTOR3 Point;
			float Padding2;
		} Data;
	};
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
	GameTerrain();
	~GameTerrain();

	void SaveTerrain(wstring saveFile = L"");
	void LoadTerrain(wstring saveFile = L"");
	void OpenTexture(wstring file = L"");
	void SetTexture(UINT num, wstring file = L"");

	void Update();

	void Render();
	void PreRender();
	void PreRender2();

	void ImGuiRender();

	bool Intersect(D3DXVECTOR3 cam, D3DXVECTOR3 camDir, float& dis);

	wstring GetFileName() { return terrainFile; }

	UINT Width() { return width; }
	UINT Height() { return height; }

	bool EditMode() { return editMode; }
	void EditMode(bool val);

	void EditTerrain();

	bool GetHeight(float x, float z, float& y);
	bool GetHeight(D3DXVECTOR3& pos);

private:
	void FirstInit(UINT width = 255, UINT height = 255);
	void Init(UINT width = 255, UINT height = 255);
	void CreateNormal();
	void CreateBuffer();

	void Clear();

	void TreeFile(wstring file = L"");

	float SplatColor(float origin, float brush, float inten);
	D3DXCOLOR SplatColor(D3DXCOLOR origin, D3DXCOLOR brush, float inten);

	void QuadTreeUpdate();

private:
	typedef VertexColorTextureNormal VertexType;

	//base
	UINT width, height;

	VertexType* vertices;
	UINT* indices;

	UINT vertexSize, indexSize;

	ID3D11Buffer* vertexBuffer, *indexBuffer;

	Texture* tex1;
	Texture* tex2;
	Texture* tex3;
	Texture* tex4;

	Shader* shader;
	Shader* shader2;
	Shader* shader3;

	Material* material;

	WorldBuffer* buffer;
	TerrainBuffer* terrainBuffer;

	//for texture
	wstring textureFile;

	//for save
	wstring terrainFile;

	//for edit
	bool editMode;

	UINT editType;

	QuadStruct quadRoot;
	QuadStruct quadruple[4];

	QuadStruct sexdecuple[16];


	D3DXVECTOR3 selTer;

	bool changed;
	float heightSet;
	float power;

	UINT widthEdit, heightEdit;

	//splatting
	D3DXCOLOR splat;

	//for tree
	TreeBuffer* treeBuffer;
	float offset, windPower;

	//edit-tree
	class Tree* treeSet;
	vector<class Tree*> trees;
	bool treeDisposed;
	UINT treeNum;
	float treeScale;
	float treeDelay;

	//Water
	class Water* water;
	bool useWater;

	//Light
	class PointLight* pointLight;
};