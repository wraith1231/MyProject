#include "stdafx.h"
#include "ModelMeshPart.h"

ModelMeshPart::ModelMeshPart()
{
}

ModelMeshPart::~ModelMeshPart()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

void ModelMeshPart::Binding()
{
	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	//1. Vertex Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(ModelVertexType) * vertices.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &vertices[0];

		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//2. Index Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indices.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &indices[0];

		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}

void ModelMeshPart::Render()
{
	UINT stride = sizeof(ModelVertexType);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	material->SetBuffer();
	D3D::GetDC()->DrawIndexed(indices.size(), 0, 0);
}

void ModelMeshPart::CheckMaxVer(D3DXVECTOR3 & max)
{
	for (size_t i = 0; i < vertices.size(); i++)
	{
		if (max.x < vertices[i].position.x) max.x = vertices[i].position.x;
		if (max.y < vertices[i].position.y) max.y = vertices[i].position.y;
		if (max.z < vertices[i].position.z) max.z = vertices[i].position.z;
	}
}

void ModelMeshPart::CheckMinVer(D3DXVECTOR3 & min)
{
	for (size_t i = 0; i < vertices.size(); i++)
	{
		if (min.x > vertices[i].position.x) min.x = vertices[i].position.x;
		if (min.y > vertices[i].position.y) min.y = vertices[i].position.y;
		if (min.z > vertices[i].position.z) min.z = vertices[i].position.z;
	}
}

void ModelMeshPart::CheckMaxMin(D3DXVECTOR3 & max, D3DXVECTOR3 & min)
{
	for (size_t i = 0; i < vertices.size(); i++)
	{
		if (max.x < vertices[i].position.x) max.x = vertices[i].position.x;
		if (max.y < vertices[i].position.y) max.y = vertices[i].position.y;
		if (max.z < vertices[i].position.z) max.z = vertices[i].position.z;

		if (min.x > vertices[i].position.x) min.x = vertices[i].position.x;
		if (min.y > vertices[i].position.y) min.y = vertices[i].position.y;
		if (min.z > vertices[i].position.z) min.z = vertices[i].position.z;
	}
}

void ModelMeshPart::Clone(void ** clone)
{
	ModelMeshPart* part = new ModelMeshPart();

	part->materialName = materialName;

	part->vertices.assign(vertices.begin(), vertices.end());
	part->indices.assign(indices.begin(), indices.end());

	*clone = part;
}