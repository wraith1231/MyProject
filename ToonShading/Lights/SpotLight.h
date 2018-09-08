#pragma once

namespace Objects
{
	class BoundingBox;
	class Ray;
}

struct SpotLightSave
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Color;
	D3DXVECTOR3 Direction;

	float RangeRcp;
	float CosOuter;
	float CosInner;
};

class SpotLight
{
public:
	SpotLight(ExecuteValues* values = NULL);
	~SpotLight();

	UINT AddSpotLight(D3DXVECTOR3 Position = D3DXVECTOR3(0, 0, 0), D3DXVECTOR3 Color = D3DXVECTOR3(1, 1, 1), D3DXVECTOR3 Dir = D3DXVECTOR3(1, 0, 0),
		float Range = 10.0f, float CosOuter = 1.0f, float CosInner = 0.0f, bool add = false);
	UINT SpotLightSize();

	void PreRender();
	void PreRender(bool val);
	void LightRender();

	void ImGuiRender();

	bool LightUse(UINT num, SpotLightSave& data);

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
			Data.RangeRcp = 10.0f;
			Data.Direction = D3DXVECTOR3(0, -1.0f, 0);
			Data.CosOuter = 15.0f;
			Data.Color = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			Data.CosAttnRange = 5.0f;
		}

		struct Struct
		{
			D3DXVECTOR3 Position;
			float RangeRcp;

			D3DXVECTOR3 Direction;
			float CosOuter;

			D3DXVECTOR3 Color;
			float CosAttnRange;
		} Data;
	};
	class DSBuffer : public ShaderBuffer
	{
	public:
		DSBuffer() : ShaderBuffer(&Data, sizeof(Struct))
		{
			Data.SinAngle = 0.0f;
			Data.CosAngle = 0.0f;
		}

		struct Struct
		{
			float SinAngle;
			float CosAngle;
			float Padding[2];

		} Data;
	};

public:
	ExecuteValues* values;
	Objects::BoundingBox* box;

	Buffer* buffer;
	DSBuffer* dsBuffer;

	bool lightSelect;
	UINT selectNum;

	vector<SpotLightSave> lights;

	Shader* psShader;
	ID3D11BlendState* blend;
	ID3D11RasterizerState* rasterize;
};