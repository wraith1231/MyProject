#pragma once

struct ExecuteValues;

typedef VertexTexture VertexGrass;

class Grass
{
public:
	Grass(wstring fileName, ExecuteValues* values = NULL);
	~Grass();

	void ShadowRender();
	void PreRender();

	void CreateBuffer();
	void BufferUpdate();

	void AddGrass(D3DXVECTOR3 position, D3DXVECTOR2 size);
	void DeleteGrass(D3DXVECTOR3 center, float distance, UINT type);
	UINT GrassSize();

	vector<VertexGrass> GrassData() { return grassData; }
	void GrassDataCopy(vector<VertexGrass>& vec);
	void GrassDataPaste(vector<VertexGrass> vec);

	wstring FileName();

private:
	Shader* shader;
	Shader* shadowShader;

	ID3D11Buffer* buffer;

	vector<VertexGrass> grassData;

	Texture* texture;
	
};