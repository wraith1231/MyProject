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
	class LightBuffer : public ShaderBuffer
	{
	public:
		LightBuffer() : ShaderBuffer(&Data, sizeof(Struct))
		{
			Data.BufferRender = 0;
		}

		struct Struct
		{
			UINT BufferRender;
			float Padding[3];
		} Data;
	};

	class DirBuffer : public ShaderBuffer
	{
	public:
		DirBuffer() : ShaderBuffer(&Data, sizeof(Struct))
		{
			Data.AmbientDown = D3DXVECTOR3(-1.0, -1.0, -1.0);
			Data.AmbientRange = D3DXVECTOR3(1.0, 1.0, 1.0);
			Data.DirToLight = D3DXVECTOR3(0, 1, 0);
			Data.DirColor = D3DXVECTOR3(1, 1, 1);
		}

		struct Struct
		{
			D3DXVECTOR3 AmbientDown;
			float Padding1;
			D3DXVECTOR3 AmbientRange;
			float Padding2;
			D3DXVECTOR3 DirToLight;
			float Padding3;
			D3DXVECTOR3 DirColor;
			float Padding4;
		} Data;
	};

public:
	ToonShading(ExecuteValues* values);
	~ToonShading();

	void Update();

	void ShadowRender();
	void PreRender();
	void LightRender();
	void EdgeRender();
	void AARender();
	void ImGuiRender();

	void ResizeScreen();

private:
	class Model* edgeModel;
	class Model* aaModel;
	class Model* lightModel;
	D3DXMATRIX view;

	RenderTarget* normalRT;
	RenderTarget* depthRT;
	RenderTarget* diffuseRT;
	RenderTarget* lightMeshRT;
	RenderTarget* lightRT;
	RenderTarget* AART;

	Orthographic* projection;

	WorldBuffer* worldBuffer;

	Shader* edgeShader;
	Shader* lightShader;
	Shader* aaShader;

	Buffer* buffer;
	LightBuffer* lightBuffer;

	float xplus, yplus, zplus;

	ID3D11DepthStencilState* writeLessStencilMask;

	Shader* dirLight;
	DirBuffer* dirBuffer;
};