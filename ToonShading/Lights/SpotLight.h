#pragma once

#define SPOTLIGHTSIZE 32
namespace Objects
{
	class BoundingBox;
	class Ray;
}

struct SpotLightSave
{
	float InnerAngle;
	float OuterAngle;

	D3DXVECTOR3 Position;

	D3DXVECTOR3 Color;

	D3DXVECTOR3 Direction;
};

class SpotLight
{
public:
	SpotLight();
	~SpotLight();

	UINT AddSpotLight(D3DXVECTOR3 position = D3DXVECTOR3(0, 0, 0), D3DXVECTOR3 color = D3DXVECTOR3(1, 1, 1), D3DXVECTOR3 dir = D3DXVECTOR3(0, -1, 0),
		float inner = 0.75f, float outer = 0.25f);
	UINT SpotLightSize();
	UINT SpotLightMaxSize() { return SPOTLIGHTSIZE; }

	void Render();
	void Render(bool val);

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
			for (size_t i = 0; i < SPOTLIGHTSIZE; i++)
			{
				Data.Light[i].Use = 0;
				Data.Light[i].InnerAngle = 0.75f;
				Data.Light[i].OuterAngle = 0.25f;
				Data.Light[i].Position = D3DXVECTOR3(0, 0, 0);
				Data.Light[i].Color = D3DXVECTOR3(1, 1, 1);
				Data.Light[i].Direction = D3DXVECTOR3(0, -1, 0);
			}

			Data.Count = 0;
		}

		struct Lights
		{
			UINT Use;
			float InnerAngle;
			float OuterAngle;
			float Padding1;

			D3DXVECTOR3 Position;
			float Padding2;

			D3DXVECTOR3 Color;
			float Padding3;

			D3DXVECTOR3 Direction;
			float Padding4;
		};

		struct Struct
		{
			Lights Light[SPOTLIGHTSIZE];

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