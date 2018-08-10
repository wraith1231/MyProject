#pragma once

class Fog
{
public:
	Fog();
	~Fog();

	void Update();
	void Render();
	void ImGuiRender();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Color = D3DXCOLOR(0, 0, 0, 1);

			Data.Start = 0.5f;
			Data.End = 10.0f;
		}

		struct Struct
		{
			D3DXCOLOR Color;

			float Start;
			float End;
			float Padding[2];
		} Data;
	};

private:
	Buffer* buffer;
};