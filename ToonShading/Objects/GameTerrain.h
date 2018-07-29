#pragma once

class GameTerrain
{
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


private:
	void Init();
	void CreateNormal();
	void CreateBuffer();

private:
	typedef VertexColorTextureNormal VertexType;

	UINT width, height;

	VertexType* vertices;
	UINT* indices;

	D3DXVECTOR3 intersectCheck[4];

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
	wstring textureFile;

	wstring terrainFile;
};