#pragma once

#define POINTLIGHTSIZE 32
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
	PointLight();
	~PointLight();

	UINT AddPointLight(D3DXVECTOR3 position = D3DXVECTOR3(0, 0, 0), D3DXVECTOR3 color = D3DXVECTOR3(1, 1, 1), float intensity =  1.0f, float range = 1.0f);
	UINT PointLightSize();
	UINT PointLightMaxSize() { return POINTLIGHTSIZE; }

	void PreRender();
	void PreRender(bool val);

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
			for (Lights light : Data.Light)
			{
				light.Use = 0;
				light.Intensity = 2.0f;
				light.Range = 10.0f;

				light.Position = D3DXVECTOR3(0, 0, 0);
				light.Color = D3DXVECTOR3(0, 0, 0);
			}

			Data.Count = 0;
		}

		struct Lights
		{
			UINT Use = 0;				//0이면 사용하지 않음, 1이면 사용
			float Intensity = 1.0f;
			float Range = 1.0f;
			float Padding1;

			D3DXVECTOR3 Position = D3DXVECTOR3(0, 0, 0);
			float Padding2;
			D3DXVECTOR3 Color = D3DXVECTOR3(1, 1, 1);
			float Padding3;
		};

		struct Struct
		{
			Lights Light[POINTLIGHTSIZE];

			int Count;
			float Padding[3];
		} Data;
	};
public:
	Objects::BoundingBox* box;

	Buffer* buffer;

	bool lightSelect;
	UINT selectNum;

};