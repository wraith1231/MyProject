#pragma once

class Orthographic;

class ToonShading2 : public Execute
{
public:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Struct))
		{
			Data.Count = 0;
			Data.Width = 0.0f;
			Data.Height = 0.0f;
		}

		struct Struct
		{
			UINT Count;
			float Width;
			float Height;
			float Padding;
		}Data;
	};

public:
	ToonShading2(ExecuteValues* values);
	~ToonShading2();

	void Update();

	void PreRender();
	void PreRender2();
	void Render();
	void PostRender();
	void PostRender2();
	void ImGuiRender();

	void ResizeScreen();

private:
	class Model* model;
	D3DXMATRIX view;

	RenderTarget* ndRT;
	RenderTarget* realRT;
	Orthographic* projection;

	WorldBuffer* worldBuffer;

	Shader* shader;
	Buffer* buffer;

};