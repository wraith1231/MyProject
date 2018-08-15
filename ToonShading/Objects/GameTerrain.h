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

//물까지가 초급, 깊이~알파가 중급, 지연 렌더링이 고급
//deferred Rendering - 이건 조명도 다 후면에서 계산함
//ambient occlusion
//anti aliasing
//raster tek - 지면에서 geometry shader가 있는데 이게 비나 풀같은거 구현하는것
//깊이 버퍼로 할때 제일 볼만한게 눈쌓이는 표현같은거
//눈에 발자국 그리는건 ia 붙는게 기초데이터(input assembly), IA 다음에 VS 그다음
//HS(Hull Shader) HS가 끝나면 TS(Tessellation Shader), DS(Domain Shader, 영역 채우기),
//GS(Geometry Shader 면마다 채우는 애) 순으로 진행 - 이게 화면 나가기 전까지의 점 정보, 
//여기서 하나 더 붙는게 SO(String Output, 여기까지 진행된 정보를 써야될 때 사용)
//SO Buffer에서 DX에 세팅하면 처리된 결과가 그 Buffer에 리턴이 되는데 그걸 이용해
//다시 IA~SO 진행이 가능 - 여기까지가 전처리고 이제 RS(Rasterize State), Depth,
//Blending, ...등 붙고 Clip이 일어남(그리지 않을 영역 자르기 쉐이더 clip), near~far를
//이용해 clip이 일어남 / 그리고 마지막이 PS <- cbuffer, texture, sampler등이 여기서 붙음
//이렇게 많은 작업이 있어서 cbuffer 공유가 불가능
//9때는 스테이지라고 불러서 VS에서 처리한걸 PS로 넘기면 그걸 Texture에 쓴 뒤
//그 텍스쳐의 픽셀을 읽고 그 결과로 높이를 조정했음(VTF?)

//SmoothStep - 선형보간보다 더 부드러운 보간
//게임업계에서 자주 쓰는게 선형보간과 퍼포먼스 죽이고 쓰는게
//에르미트? 헤르미트? Hermite 보간법 - 얘도 dx 기본 지원이긴 함
//그외 catmullrom, bezier 등이 있음
//최소한 어떻게 동작하는지에 대해서는 알아둘 것
//울타리 같은데 가둬둘 때 볼록 다각형을 이용함
//오목다각형은 볼록과 반대, 위치가 안에 있는지 없는지 판별은 내부에서
//선을 그어서 닿는 점이 홀수면 내부에, 짝수면 밖에 있는 것
//gjk는 볼록 다각형에서 충돌 관련된 알고리즘
//이건 충돌 깊이까지 알 수 있어서 자주 쓴다
//AABB OBB를 합친것
//라그랑주 다항식도 게임에서 은근 쓰임 - 커브같은 곳에서