#pragma once

namespace Objects
{
	class BoundingBox;
	class Ray;
}

struct PointLightSave
{
	float intenstiy;
	float range;

	D3DXVECTOR3 Position;
	D3DXVECTOR3 Color;
};

class PointLight
{
public:
	PointLight(ExecuteValues* values = NULL);
	~PointLight();

	UINT AddPointLight(D3DXVECTOR3 position = D3DXVECTOR3(0, 0, 0), D3DXVECTOR3 color = D3DXVECTOR3(1, 1, 1), float intensity =  1.0f, float range = 10.0f, bool add = false);
	UINT PointLightSize();

	void PreRender();
	void PreRender(bool val);
	void LightRender();

	void ImGuiRender();

	bool LightUse(UINT num, PointLightSave& data);

	bool LightSelect() { return lightSelect; }
	void LightSelect(bool val, UINT num) { lightSelect = val; selectNum = num; }
	void LightSelect(Objects::Ray* ray);

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Position = D3DXVECTOR3(0, 0, 0);
			Data.RangeRcp = 1.0f;
			Data.Color = D3DXVECTOR3(1, 1, 1);
		}

		struct Struct
		{
			D3DXVECTOR3 Position;
			float RangeRcp;

			D3DXVECTOR3 Color;
			float Padding1;
		} Data;
	};

public:
	ExecuteValues* values;
	Objects::BoundingBox* box;

	Buffer* buffer;

	bool lightSelect;
	UINT selectNum;

	vector<PointLightSave> lights;

	Shader* psShader;
	ID3D11BlendState* blend;
	ID3D11RasterizerState* rasterize;
};