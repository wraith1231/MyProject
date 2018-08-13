#pragma once

struct FogSave
{
	D3DXCOLOR Color;
	float Start;
	float End;
	UINT Use;
};

class Fog
{
public:
	Fog();
	~Fog();

	void Update();
	void DiffuseRender();
	void ImGuiRender();

	void SetUse(bool val);

	FogSave GetFogData();
	void SetFogData(FogSave fog);

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Color = D3DXCOLOR(0, 0, 0, 1);

			Data.Start = 0.0f;
			Data.End = 1.0f;
		}

		struct Struct
		{
			D3DXCOLOR Color;

			float Start;
			float End;
			UINT Use;
			float Padding;
		} Data;
	};

private:
	Buffer* buffer;
};