#pragma once

class PointLight
{
public:
	PointLight();
	~PointLight();

	bool AddPointLight(D3DXVECTOR3 position, D3DXVECTOR3 color, float intensity, float range);

	void Render();

	void ImGuiRender();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{

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
			Lights Light[16];
		} Data;
	};
public:
	Buffer* buffer;

};