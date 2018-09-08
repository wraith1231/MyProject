#pragma once

namespace Objects
{
	class BoundingBox;
	class Ray;
}

struct CapsuleLightSave
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Color;
	D3DXVECTOR3 Direction;

	float RangeRcp;
	float Length;
	float HalfSegmentation;
	float Range;
};

class CapsuleLight
{
public:
	CapsuleLight(ExecuteValues* values = NULL);
	~CapsuleLight();

	UINT AddCapsuleLight(D3DXVECTOR3 Position = D3DXVECTOR3(0, 0, 0), D3DXVECTOR3 Color = D3DXVECTOR3(1, 1, 1), D3DXVECTOR3 Dir = D3DXVECTOR3(1, 0, 0), 
		float RangeRcp = 10.0f, float Length = 10.0f, float HalfSeg = 1.0f, float Ran = 10.0f, bool add = false);
	UINT CapsuleLightSize();

	void PreRender();
	void PreRender(bool val);
	void LightRender();

	void ImGuiRender();

	bool LightUse(UINT num, CapsuleLightSave& data);

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
			Data.Color = D3DXVECTOR3(1, 1, 1);
			Data.Direction = D3DXVECTOR3(1.0f, 0, 0);
			Data.Length = 10.0f;
		}

		struct Struct
		{
			D3DXVECTOR3 Position;
			float RangeRcp;

			D3DXVECTOR3 Direction;
			float Length;

			D3DXVECTOR3 Color;
			float Padding;
		} Data;
	};
	class DSBuffer : public ShaderBuffer
	{
	public:
		DSBuffer() : ShaderBuffer(&Data, sizeof(Struct))
		{
			Data.HalfSegmentation = 1.0f;
			Data.Range = 10.0f;
		}

		struct Struct
		{
			float HalfSegmentation;
			float Range;
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

	vector<CapsuleLightSave> lights;

	Shader* psShader;
	ID3D11BlendState* blend;
	ID3D11RasterizerState* rasterize;
};