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
	class TargetBuffer : public ShaderBuffer
	{
	public:
		TargetBuffer() : ShaderBuffer(&Data, sizeof(Struct))
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
	class SSAOBuffer : public ShaderBuffer
	{
	public:
		SSAOBuffer() : ShaderBuffer(&Data, sizeof(Struct))
		{
			Data.Sample = 16;
			Data.Intensity = 1.0f;
			Data.Scale = 2.5f;
			Data.Bias = 0.05f;

			Data.Radius = 0.02f;
			Data.MaxDistance = 0.07f;

			Data.Moo3 = D3DXVECTOR3(0.1031f, 0.11369f, 0.13787f);
		}

		struct Struct
		{
			UINT Sample;
			float Intensity;
			float Scale;
			float Bias;

			float Radius;
			float MaxDistance;
			float Padding1[2];

			D3DXVECTOR3 Moo3;
			float Padding2;
		} Data;
	};

public:
	ToonShading(ExecuteValues* values);
	~ToonShading();

	void Update();

	void ShadowRender();
	void PreRender();
	void LightRender();
	void SSAORender();
	void EdgeRender();
	void AARender();
	void ImGuiRender();

	void ResizeScreen();

private:

	//Shadow Mapping용
	RenderTarget* shadowRT;

	//Deferred Rendering용
	RenderTarget* normalRT, *depthRT, *diffuseRT;

	//Lighting용
	RenderTarget* lightRT;

	//SSAO용
	RenderTarget* ssaoRT;

	//Edge용
	RenderTarget* edgeRT;

	Shader* ssaoShader;
	Shader* edgeShader;
	Shader* aaShader;

	//Projection값(near, far, width, height) 쉐이더 전달용
	Buffer* buffer;

	//Render Target 보여주기용
	TargetBuffer* lightBuffer;

	//SSAO용 버퍼
	SSAOBuffer* ssaoBuffer;

	ID3D11DepthStencilState* writeLessStencilMask;

	//Lighting - Directional Light
	Shader* dirLight;
	DirBuffer* dirBuffer;
};