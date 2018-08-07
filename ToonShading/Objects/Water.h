#pragma once

struct WaterStruct
{
	D3DXVECTOR2 TextureScale;
	float WaveFrequancy;
	float WaveAmplitude;

	float Time;
	float BumpScale;
	D3DXVECTOR2 BumpSpeed;

	float Height;

	D3DXCOLOR DeepColor;

	D3DXCOLOR ShallowColor;

	D3DXCOLOR ReflectionColor;

	float ReflectionAmount;
	float ReflectionBlur;
	float FresnelPower;
	float FresnelBias;

	float HDRMultiplier;
	float WaterAmount;
};

class Water
{
private:
	class WaterVSBuffer : public ShaderBuffer
	{
	public:
		WaterVSBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.TextureScale = D3DXVECTOR2(1.0f, 1.0f);
			Data.WaveFrequancy = 0.4f;
			Data.WaveAmplitude = 0.3f;

			Data.Time = 0.0f;
			Data.BumpScale = 0.5f;
			Data.BumpSpeed = D3DXVECTOR2(0.01f, 0.01f);

			Data.Height = 5.0f;
		}

		struct Struct
		{
			D3DXVECTOR2 TextureScale;
			float WaveFrequancy;
			float WaveAmplitude;

			float Time;
			float BumpScale;
			D3DXVECTOR2 BumpSpeed;

			float Height;
			float Padding[3];
		} Data;
	};
	class WaterPSBuffer : public ShaderBuffer
	{
	public:
		WaterPSBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.DeepColor = D3DXCOLOR(0.0f, 0.0f, 0.3f, 1.0f);
			Data.ShallowColor = D3DXCOLOR(0.4f, 0.5f, 1.0f, 1.0f);
			Data.ReflectionColor = D3DXCOLOR(0.8f, 0.9f, 0.9f, 1.0f);

			Data.ReflectionAmount = 1.3f;
			Data.ReflectionBlur = 0.5f;
			Data.FresnelPower = 3.0f;
			Data.FresnelBias = 0.1f;

			Data.HDRMultiplier = 2.0f;
			Data.WaterAmount = 2.0f;
		}

		struct Struct
		{
			D3DXCOLOR DeepColor;
			
			D3DXCOLOR ShallowColor;

			D3DXCOLOR ReflectionColor;

			float ReflectionAmount;
			float ReflectionBlur;
			float FresnelPower;
			float FresnelBias;

			float HDRMultiplier;
			float WaterAmount;
			float Padding[2];
		} Data;
	};

public:
	Water(UINT width, UINT height);
	~Water();

	void Update();
	void Render();
	void ImGuiRender();

	WaterStruct GetWaterParameter();
	void SetWaterParameter(WaterStruct param);

private:
	void Init();

private:
	typedef VertexTexture VertexType;

	UINT width, height;
	VertexType* vertices;
	UINT* indices;

	UINT vertexSize, indexSize;
	ID3D11Buffer* vertexBuffer, *indexBuffer;

	Shader* shader;
	Material* material;

	WaterVSBuffer* vsBuffer;	//2¹ø
	WaterPSBuffer* psBuffer;	//2¹ø
	WorldBuffer* worldBuffer;

	ID3D11BlendState* blendState[2];
};