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
struct ExecuteValues;

class QuadTree
{
public:
	QuadTree()
	{
		root = NULL;
		for (size_t i = 0; i < 4; i++)
			child[i] = NULL;
		LB = LT = RB = RT = D3DXVECTOR3(0, 0, 0);
	}
	~QuadTree()
	{
		for (size_t i = 0; i < 4; i++)
		{
			SAFE_DELETE(child[i]);
			child[i] = NULL;
		}
	}

	bool FindQuad(D3DXVECTOR3 cam, D3DXVECTOR3 camDir, float& dis, D3DXVECTOR3& LB, D3DXVECTOR3& LT, D3DXVECTOR3& RB, D3DXVECTOR3& RT);
	void ChildMake();

	D3DXVECTOR3 LB, LT, RB, RT;

	QuadTree* root;
	QuadTree* child[4];
};

namespace Objects
{
	class Ray;
}

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
	GameTerrain(ExecuteValues* value = NULL);
	~GameTerrain();

	void SaveTerrain(wstring saveFile = L"");
	void LoadTerrain(wstring saveFile = L"");
	void OpenTexture(wstring file = L"");
	void SetTexture(UINT num, wstring file = L"");

	void Update();

	void PreRender();
	void ImGuiRender();

	bool Intersect(D3DXVECTOR3 cam, D3DXVECTOR3 camDir, float& dis);

	wstring GetFileName() { return terrainFile; }

	UINT Width() { return width; }
	UINT Height() { return height; }

	bool EditMode() { return editMode; }
	void EditMode(bool val);

	void EditTerrain();

	bool PointLightDispose() { return pointLightDispose; }
	void PointLightDispose(D3DXVECTOR3 pos);
	void PointLightSelect(Objects::Ray* ray);

	bool SpotLightDispose() { return spotLightDispose; }
	void SpotLightDispose(D3DXVECTOR3 pos);
	void SpotLightSelect(Objects::Ray* ray);

	bool GetHeight(float x, float z, float& y);
	bool GetHeight(D3DXVECTOR3& pos);

private:
	void FirstInit(UINT width = 255, UINT height = 255);
	void Init(UINT width = 255, UINT height = 255);
	void CreateNormal();
	void CreateBuffer();

	void QuadTreeMake(UINT width = 255, UINT height = 255);

	void Clear();

	void TreeFile(wstring file = L"");

	float SplatColor(float origin, float brush, float inten);
	D3DXCOLOR SplatColor(D3DXCOLOR origin, D3DXCOLOR brush, float inten);

	void QuadTreeUpdate();

private:
	typedef VertexColorTextureNormal VertexType;

	//base
	ExecuteValues* values;

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

	QuadTree* quadTreeRoot;

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
	bool pointLightDispose, spotLightDispose;
	bool pointLightSelect, spotLightSelect;

	class PointLight* pointLight;
	class SpotLight* spotLight;

	//Fog
	bool useFog;
	class Fog* fog;
};

//�������� �ʱ�, ����~���İ� �߱�, ���� �������� ���
//deferred Rendering - �̰� ���� �� �ĸ鿡�� �����
//ambient occlusion
//anti aliasing
//raster tek - ���鿡�� geometry shader�� �ִµ� �̰� �� Ǯ������ �����ϴ°�
//���� ���۷� �Ҷ� ���� �����Ѱ� �����̴� ǥ��������
//���� ���ڱ� �׸��°� ia �ٴ°� ���ʵ�����(input assembly), IA ������ VS �״���
//HS(Hull Shader) HS�� ������ TS(Tessellation Shader), DS(Domain Shader, ���� ä���),
//GS(Geometry Shader �鸶�� ä��� ��) ������ ���� - �̰� ȭ�� ������ �������� �� ����, 
//���⼭ �ϳ� �� �ٴ°� SO(String Output, ������� ����� ������ ��ߵ� �� ���)
//SO Buffer���� DX�� �����ϸ� ó���� ����� �� Buffer�� ������ �Ǵµ� �װ� �̿���
//�ٽ� IA~SO ������ ���� - ��������� ��ó���� ���� RS(Rasterize State), Depth,
//Blending, ...�� �ٰ� Clip�� �Ͼ(�׸��� ���� ���� �ڸ��� ���̴� clip), near~far��
//�̿��� clip�� �Ͼ / �׸��� �������� PS <- cbuffer, texture, sampler���� ���⼭ ����
//�̷��� ���� �۾��� �־ cbuffer ������ �Ұ���
//9���� ����������� �ҷ��� VS���� ó���Ѱ� PS�� �ѱ�� �װ� Texture�� �� ��
//�� �ؽ����� �ȼ��� �а� �� ����� ���̸� ��������(VTF?)

//SmoothStep - ������������ �� �ε巯�� ����
//���Ӿ��迡�� ���� ���°� ���������� �����ս� ���̰� ���°�
//������Ʈ? �츣��Ʈ? Hermite ������ - �굵 dx �⺻ �����̱� ��
//�׿� catmullrom, bezier ���� ����
//�ּ��� ��� �����ϴ����� ���ؼ��� �˾Ƶ� ��
//��Ÿ�� ������ ���ֵ� �� ���� �ٰ����� �̿���
//����ٰ����� ���ϰ� �ݴ�, ��ġ�� �ȿ� �ִ��� ������ �Ǻ��� ���ο���
//���� �׾ ��� ���� Ȧ���� ���ο�, ¦���� �ۿ� �ִ� ��
//gjk�� ���� �ٰ������� �浹 ���õ� �˰���
//�̰� �浹 ���̱��� �� �� �־ ���� ����
//AABB OBB�� ��ģ��
//��׶��� ���׽ĵ� ���ӿ��� ���� ���� - Ŀ�갰�� ������