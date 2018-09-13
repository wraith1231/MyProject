#include "stdafx.h"
#include "Grass.h"

Grass::Grass(wstring fileName, ExecuteValues* values)
	: buffer(NULL)
{
	shader = new Shader(Shaders + L"999_Grass.hlsl");
	shader->CreateGeometryShader();
	shadowShader = new Shader(Shaders + L"999_Grass.hlsl", "VS_Shadow", "PS_Shadow");
	shadowShader->CreateGeometryShader("GS_Shadow");
	
	texture = new Texture(fileName);
	
}

Grass::~Grass()
{
	SAFE_DELETE(shadowShader);
	SAFE_DELETE(shader);
}

void Grass::ShadowRender()
{
	if (grassData.size() == 0)
		return;

	shadowShader->Render();
	texture->SetShaderResource(0);
	texture->SetShaderSampler(0);

	UINT stride = sizeof(VertexGrass);
	UINT offset = 0;

	D3D::GetDC()->IASetInputLayout(NULL);
	D3D::GetDC()->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);

	D3D::GetDC()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);
	D3D::GetDC()->Draw(grassData.size(), 0);

	Shader::ClearShader();
}

void Grass::PreRender()
{
	if (grassData.size() == 0)
		return;

	shader->Render();
	texture->SetShaderResource(0);
	texture->SetShaderSampler(0);
	
	UINT stride = sizeof(VertexGrass);
	UINT offset = 0;

	D3D::GetDC()->IASetInputLayout(NULL);
	D3D::GetDC()->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);
	D3D::GetDC()->Draw(grassData.size(), 0);

	Shader::ClearShader();
}

void Grass::CreateBuffer()
{
	SAFE_RELEASE(buffer);

	if (grassData.size() == 0) return;

	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexGrass) * grassData.size();
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = &grassData[0];

	HRESULT hr;
	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &buffer);
	assert(SUCCEEDED(hr));
}

void Grass::BufferUpdate()
{
	D3D::GetDC()->UpdateSubresource
	(
		buffer, 0, NULL, &grassData[0], sizeof(VertexGrass) * grassData.size(), 0
	);
}

void Grass::AddGrass(D3DXVECTOR3 position, D3DXVECTOR2 size)
{
	VertexGrass temp;
	temp.position = position;
	temp.uv = size;

	grassData.push_back(temp);

	CreateBuffer();
}

void Grass::DeleteGrass(D3DXVECTOR3 center, float distance, UINT type)
{
	//type == 0 -> circle, 1 -> Square
	if (type == 0)
	{
		for (UINT i = 0; i < grassData.size();)
		{
			D3DXVECTOR2 dist = D3DXVECTOR2(center.x - grassData[i].position.x, center.z - grassData[i].position.z);

			if (D3DXVec2Length(&dist) < distance)
			{
				grassData.erase(grassData.begin() + i);
			}
			else i++;
		}

		CreateBuffer();
	}
	else if (type == 1)
	{
		for (UINT i = 0; i < grassData.size();)
		{
			D3DXVECTOR2 max, min;
			max = D3DXVECTOR2(center.x - distance, center.z - distance);
			max = D3DXVECTOR2(center.x - distance, center.z - distance);

			bool b = true;
			b &= (max.x <= grassData[i].position.x && min.x >= grassData[i].position.x);
			b &= (max.y <= grassData[i].position.z && min.y >= grassData[i].position.z);

			if (b == false)
			{
				grassData.erase(grassData.begin() + i);
			}
			else i++;
		}

		CreateBuffer();
	}
}

UINT Grass::GrassSize()
{
	return grassData.size();
}

void Grass::GrassDataCopy(vector<VertexGrass>& vec)
{
	vec.assign(grassData.begin(), grassData.end());
}

void Grass::GrassDataPaste(vector<VertexGrass> vec)
{
	grassData.assign(vec.begin(), vec.end());
	CreateBuffer();
}

wstring Grass::FileName()
{
	return texture->GetFile();
}
