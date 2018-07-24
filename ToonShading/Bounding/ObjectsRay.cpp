#include "stdafx.h"
#include "ObjectsRay.h"

#include "ObjectsPlane.h"
#include "BoundingBox.h"

Objects::Ray::Ray()
{
	Position = D3DXVECTOR3(0, 0, 0);
	Direction = D3DXVECTOR3(0, -1, 0);
}

Objects::Ray::Ray(D3DXVECTOR3 pos, D3DXVECTOR3 dir)
{
	Position = pos;
	Direction = dir;

	VertexColor temp;
	temp.color = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	temp.position = pos;
	vertex.push_back(temp);
	temp.position = pos + dir * 50.0f;
	vertex.push_back(temp);

	index.push_back(0);
	index.push_back(1);

	buffer = new RayBuffer();

	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;
	//1. Vertex Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexColor) * vertex.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &vertex[0];

		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//2. Index Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * index.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &index[0];

		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
	worldBuffer = new WorldBuffer;
	shader = new Shader(Shaders + L"040_Objects.hlsl");
}

void Objects::Ray::Update(D3DXVECTOR3 pos, D3DXVECTOR3 dir)
{
	vertex[0].position = pos;
	vertex[1].position = pos + dir * 50.0f;
	Position = pos;
	Direction = dir;

	D3D::GetDC()->UpdateSubresource
	(
		vertexBuffer, 0, NULL, &vertex[0], sizeof(VertexColor) * vertex.size(), 0
	);
	
}

void Objects::Ray::Render()
{
	buffer->SetPSBuffer(2);
	UINT stride = sizeof(VertexColor);
	UINT offset = 0;

	worldBuffer->SetVSBuffer(1);
	shader->Render();

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	D3D::GetDC()->DrawIndexed(index.size(), 0, 0);
}

bool Objects::Ray::Intersects(BoundingBox * box)
{
	return box->Intersects(this);
}

void Objects::Ray::SetColor(D3DXCOLOR col)
{
	buffer->Data.Color = col;
}

class Objects::Ray * Objects::Ray::ConvertMouseToRay(D3DXVECTOR3 mouse, ExecuteValues* value)
{
	D3DXVECTOR3 nearPoint = mouse;
	nearPoint.z = 0.0f;
	D3DXVECTOR3 farPoint = mouse;
	farPoint.z = 1.0f;

	D3DXMATRIX pro, v, w;
	value->ViewProjection->GetProjection(pro);
	value->ViewProjection->GetView(v);
	D3DXMatrixIdentity(&w);
	nearPoint = value->Viewport->Unproject(nearPoint, pro, v, w);
	farPoint = value->Viewport->Unproject(farPoint, pro, v, w);

	D3DXVECTOR3 dir = farPoint - nearPoint;

	D3DXVec3Normalize(&dir, &dir);

	return new Ray(nearPoint, dir);
}
