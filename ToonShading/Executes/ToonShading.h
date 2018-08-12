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

	void NormalRender();
	void DepthRender();
	void DiffuseRender();

	void LightRender();
	void EdgeRender();
	void AARender();
	void ImGuiRender();

	void ResizeScreen();

private:
	class Model* edgeModel;
	class Model* aaModel;
	D3DXMATRIX view;

	RenderTarget* normalRT;
	RenderTarget* depthRT;
	RenderTarget* realRT;
	RenderTarget* lightRT;
	RenderTarget* AART;

	Orthographic* projection;

	WorldBuffer* worldBuffer;

	Shader* edgeShader;
	Shader* lightShader;
	Shader* aaShader;

	Buffer* buffer;

	float xplus, yplus, zplus;
};