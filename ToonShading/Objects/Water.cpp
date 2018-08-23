#include "stdafx.h"
#include "Water.h"

Water::Water(UINT width, UINT height)
	: width(width), height(height)
	, vertexBuffer(NULL), indexBuffer(NULL)
	, vertices(NULL), indices(NULL)
	, material(NULL), shader(NULL), shadowShader(NULL)
	, vsBuffer(NULL), psBuffer(NULL), worldBuffer(NULL)
{
	shader = new Shader(Shaders + L"999_Water.hlsl");
	shadowShader = new Shader(Shaders + L"999_Water.hlsl", "VS_Shadow", "PS_Shadow");

	worldBuffer = new WorldBuffer();
	vsBuffer = new WaterVSBuffer();
	psBuffer = new WaterPSBuffer();

	material = new Material;
	material->SetShader(shader);

	material->SetSpecularMap(Textures + L"WaveSpecular.png");
	material->SetNormalMap(Textures + L"WaveNormal.png");

	//blend
	{
		D3D11_BLEND_DESC desc;
		States::GetBlendDesc(&desc);
		States::CreateBlend(&desc, &blendState[0]);

		//desc.AlphaToCoverageEnable = true;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_ALPHA;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_DEST_ALPHA;
		//desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;
		States::CreateBlend(&desc, &blendState[1]);
	}

	vertexSize = (this->height + 1) * (this->width + 1);
	vertices = new VertexType[vertexSize];

	UINT index = 0;
	for (UINT z = 0; z <= this->height; z++)
	{
		for (UINT x = 0; x <= this->width; x++)
		{
			vertices[index].position = D3DXVECTOR3((float)x, 0, (float)z);
			vertices[index].uv = D3DXVECTOR2((float)x / (float)this->width, (float)z / (float)this->height);

			index++;
		}
	}

	indexSize = this->height * this->width * 6;
	indices = new UINT[indexSize];

	index = 0;
	for (UINT z = 0; z < this->height; z++)
	{
		for (UINT x = 0; x < this->width; x++)
		{
			indices[index + 0] = (this->width + 1) * z + x;
			indices[index + 1] = (this->width + 1) * (z + 1) + x;
			indices[index + 2] = (this->width + 1) * z + x + 1;
			indices[index + 3] = (this->width + 1) * z + x + 1;
			indices[index + 4] = (this->width + 1) * (z + 1) + x;
			indices[index + 5] = (this->width + 1) * (z + 1) + x + 1;

			index += 6;
		}
	}
	//Vertex Buffer Create
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexType) * vertexSize;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//Index Buffer Create
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexSize;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = indices;

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}

Water::~Water()
{
	for (int i = 0; i < 2; i++)
		SAFE_RELEASE(blendState[i]);

	SAFE_DELETE(shadowShader);
	SAFE_DELETE(shader);
	SAFE_DELETE(material);

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	SAFE_DELETE(vsBuffer);
	SAFE_DELETE(psBuffer);
	SAFE_DELETE(worldBuffer);
}

void Water::Update()
{
	vsBuffer->Data.Time += Time::Delta();
}

void Water::ShadowRender()
{
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	worldBuffer->SetVSBuffer(1);
	vsBuffer->SetVSBuffer(12);
	psBuffer->SetPSBuffer(12);
	material->SetShader(shadowShader);
	material->SetBuffer();

	D3D::GetDC()->DrawIndexed(indexSize, 0, 0);
}

void Water::PreRender()
{
	//D3D::GetDC()->OMSetBlendState(blendState[1], NULL, 0xFF);
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	worldBuffer->SetVSBuffer(1);
	vsBuffer->SetVSBuffer(12);
	psBuffer->SetPSBuffer(12);
	material->SetShader(shader);
	material->SetBuffer();

	D3D::GetDC()->DrawIndexed(indexSize, 0, 0);
	//D3D::GetDC()->OMSetBlendState(blendState[0], NULL, 0xFF);
}

void Water::ImGuiRender()
{
	ImGui::Begin("Water VS Buffer");

	{
		ImGui::Text("Time : %f", vsBuffer->Data.Time);
		ImGui::SliderFloat2("Texture Scale", vsBuffer->Data.TextureScale, 0.0f, 2.0f);
		ImGui::SliderFloat("Wave Frequancy", &vsBuffer->Data.WaveFrequancy, 0.0f, 3.0f);
		ImGui::SliderFloat("Wave Amplitude", &vsBuffer->Data.WaveAmplitude, 0.0f, 3.0f);
		ImGui::SliderFloat("Bump Scale", &vsBuffer->Data.BumpScale, 0.0f, 5.0f);
		ImGui::SliderFloat2("Bump Speed", vsBuffer->Data.BumpSpeed, 0.0f, 3.0f);
		ImGui::InputFloat("Height", &vsBuffer->Data.Height);
	}

	ImGui::End();

	ImGui::Begin("Water PS Buffer");

	{
		ImGui::ColorEdit3("Deep Color", psBuffer->Data.DeepColor);
		ImGui::ColorEdit3("Shallow Color", psBuffer->Data.ShallowColor);
		ImGui::ColorEdit3("Reflection Color", psBuffer->Data.ReflectionColor);

		ImGui::SliderFloat("Reflection Amount", &psBuffer->Data.ReflectionAmount, 0.0f, 5.0f);
		ImGui::SliderFloat("Reflection Blur", &psBuffer->Data.ReflectionBlur, 0.0f, 5.0f);
		ImGui::SliderFloat("Fresnel Power", &psBuffer->Data.FresnelPower, 0.0f, 5.0f);
		ImGui::SliderFloat("Fresnel Bias", &psBuffer->Data.FresnelBias, 0.0f, 5.0f);

		ImGui::SliderFloat("HDR Multiplier", &psBuffer->Data.HDRMultiplier, 0.0f, 5.0f);
		ImGui::SliderFloat("Water Amount", &psBuffer->Data.WaterAmount, 0.0f, 5.0f);
	}

	ImGui::End();
}

WaterStruct Water::GetWaterParameter()
{
	WaterStruct ret;

	//vs
	ret.TextureScale = vsBuffer->Data.TextureScale;
	ret.WaveFrequancy = vsBuffer->Data.WaveFrequancy;
	ret.WaveAmplitude = vsBuffer->Data.WaveAmplitude;
	ret.Time = 0.0f;
	ret.BumpScale = vsBuffer->Data.BumpScale;
	ret.BumpSpeed = vsBuffer->Data.BumpSpeed;
	ret.Height = vsBuffer->Data.Height;

	//ps
	ret.DeepColor = psBuffer->Data.DeepColor;
	ret.ShallowColor = psBuffer->Data.ShallowColor;
	ret.ReflectionColor = psBuffer->Data.ReflectionColor;
	ret.ReflectionAmount = psBuffer->Data.ReflectionAmount;
	ret.ReflectionBlur = psBuffer->Data.ReflectionBlur;
	ret.FresnelPower = psBuffer->Data.FresnelPower;
	ret.FresnelBias = psBuffer->Data.FresnelBias;
	ret.HDRMultiplier = psBuffer->Data.HDRMultiplier;
	ret.WaterAmount = psBuffer->Data.WaterAmount;

	return ret;
}

void Water::SetWaterParameter(WaterStruct param)
{
	//vs
	vsBuffer->Data.TextureScale = param.TextureScale;
	vsBuffer->Data.WaveFrequancy = param.WaveFrequancy;
	vsBuffer->Data.WaveAmplitude = param.WaveAmplitude;
	vsBuffer->Data.Time = param.Time;
	vsBuffer->Data.BumpScale = param.BumpScale;
	vsBuffer->Data.BumpSpeed = param.BumpSpeed;
	vsBuffer->Data.Height = param.Height;

	//ps
	psBuffer->Data.DeepColor = param.DeepColor;
	psBuffer->Data.ShallowColor = param.ShallowColor;
	psBuffer->Data.ReflectionColor = param.ReflectionColor;
	psBuffer->Data.ReflectionAmount = param.ReflectionAmount;
	psBuffer->Data.ReflectionBlur = param.ReflectionBlur;
	psBuffer->Data.FresnelPower = param.FresnelPower;
	psBuffer->Data.FresnelBias = param.FresnelBias;
	psBuffer->Data.HDRMultiplier = param.HDRMultiplier;
	psBuffer->Data.WaterAmount = param.WaterAmount;
}

void Water::Init()
{
}
