#pragma once

class Orthographic;

class ToonShading : public Execute
{
public:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Struct))
		{
			Data.Width = 0.0f;
			Data.Height = 0.0f;
			Data.Near = 0.0f;
			Data.Far = 0.0f;
		}

		struct Struct
		{
			float Width;
			float Height;
			float Near;
			float Far;
		}Data;
	};

public:
	ToonShading(ExecuteValues* values);
	~ToonShading();

	void Update();

	void PreRender();
	void PreRender2();
	void Render();
	void PostRender();
	void ImGuiRender();

	void ResizeScreen();

private:
	class Model* model;
	D3DXMATRIX view;

	RenderTarget* normalRT;
	RenderTarget* depthRT;
	RenderTarget* realRT;
	Orthographic* projection;

	WorldBuffer* worldBuffer;

	Shader* shader;
	Buffer* buffer;

};