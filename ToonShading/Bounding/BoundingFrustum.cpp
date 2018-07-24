#include "stdafx.h"
#include "BoundingFrustum.h"

#include "BoundingBox.h"
#include "BoundingSphere.h"

#include "ObjectsPlane.h"
#include "ObjectsRay.h"

Objects::BoundingFrustum::BoundingFrustum()
{
	worldBuffer = new WorldBuffer;
}

Objects::BoundingFrustum::BoundingFrustum(D3DXMATRIX matrix)
{
	SetMatrix(matrix);
	worldBuffer = new WorldBuffer;
}

Objects::BoundingFrustum::~BoundingFrustum()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(buffer);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

void Objects::BoundingFrustum::Update()
{
}

void Objects::BoundingFrustum::Render()
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

void Objects::BoundingFrustum::Render(D3DXMATRIX mat)
{
	worldBuffer->SetMatrix(mat);
	Render();
}

D3DXVECTOR3 Objects::BoundingFrustum::ComputeIntersection(Objects::Plane* plane, Objects::Ray* ray)
{
	float d = (-plane->D - D3DXVec3Dot(&plane->Normal, &ray->Position)) / D3DXVec3Dot(&plane->Normal, &ray->Direction);
	return ray->Position + (ray->Direction * d);
}

Objects::Ray* Objects::BoundingFrustum::ComputeIntersection(Objects::Plane* p1, Objects::Plane* p2)
{
	Objects::Ray* ray = new Objects::Ray();
	D3DXVec3Cross(&ray->Direction,&p1->Normal, &p2->Normal);
	D3DXVECTOR3 d = ray->Direction;
	float single = d.x * d.x + d.y * d.y + d.z * d.z;
	D3DXVECTOR3 t1 = (-p1->D * p2->Normal) + (p2->D * p1->Normal);
	D3DXVec3Cross(&ray->Position, &t1, &ray->Direction);
	ray->Position /= single;

	return ray;
}

void Objects::BoundingFrustum::SetMatrix(D3DXMATRIX value)
{
	buffer = new FruBuffer();
	shader = new Shader(Shaders + L"040_Objects.hlsl");

	matrix = value;
	
	for (UINT i = 0; i < 6; i++)
		plane[i] = new Objects::Plane();

	plane[2]->Normal.x = -value._14 - value._11;
	plane[2]->Normal.y = -value._24 - value._21;
	plane[2]->Normal.z = -value._34 - value._31;
	plane[2]->D = -value._44 - value._41;

	plane[3]->Normal.x = -value._14 + value._11;
	plane[3]->Normal.y = -value._24 + value._21;
	plane[3]->Normal.z = -value._34 + value._31;
	plane[3]->D = -value._44 + value._41;

	plane[4]->Normal.x = -value._14 + value._12;
	plane[4]->Normal.y = -value._24 + value._22;
	plane[4]->Normal.z = -value._34 + value._32;
	plane[4]->D = -value._44 + value._42;

	plane[5]->Normal.x = -value._14 - value._12;
	plane[5]->Normal.y = -value._24 - value._22;
	plane[5]->Normal.z = -value._34 - value._32;
	plane[5]->D = -value._44 - value._42;

	plane[0]->Normal.x = -value._13;
	plane[0]->Normal.y = -value._23;
	plane[0]->Normal.z = -value._33;
	plane[0]->D = -value._43;

	plane[1]->Normal.x = -value._14 + value._13;
	plane[1]->Normal.y = -value._24 + value._23;
	plane[1]->Normal.z = -value._34 + value._33;
	plane[1]->D = -value._44 + value._43;

	for (int i = 0; i < 6; i++)
	{
		D3DXVECTOR3 v = plane[i]->Normal;
		float single = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		plane[i]->Normal /= single;
		plane[i]->D /= single;
	}

	Objects::Ray* ray = ComputeIntersection(plane[0], plane[2]);
	cornerArray[0] = ComputeIntersection(plane[4], ray);
	cornerArray[3] = ComputeIntersection(plane[5], ray);
	
	ray = ComputeIntersection(plane[3], plane[0]);
	cornerArray[1] = ComputeIntersection(plane[4], ray);
	cornerArray[2] = ComputeIntersection(plane[5], ray);

	ray = ComputeIntersection(plane[2], plane[1]);
	cornerArray[4] = ComputeIntersection(plane[4], ray);
	cornerArray[7] = ComputeIntersection(plane[5], ray);

	ray = ComputeIntersection(plane[1], plane[3]);
	cornerArray[5] = ComputeIntersection(plane[4], ray);
	cornerArray[6] = ComputeIntersection(plane[5], ray);
	
	for (UINT i = 0; i < 8; i++)
	{
		VertexColor temp;
		temp.color = D3DXCOLOR(0, 0, 0, 0);
		temp.position = cornerArray[i];
		vertex.push_back(temp);
	}

	index.push_back(0); index.push_back(1);
	index.push_back(1); index.push_back(2);
	index.push_back(2); index.push_back(3);
	index.push_back(3); index.push_back(0);

	index.push_back(4); index.push_back(5);
	index.push_back(5); index.push_back(6);
	index.push_back(6); index.push_back(7);
	index.push_back(7); index.push_back(4);

	index.push_back(0); index.push_back(4);
	index.push_back(1); index.push_back(5);
	index.push_back(2); index.push_back(6);
	index.push_back(3); index.push_back(7);

	UINT vertexCount = vertex.size();
	UINT indexCount = index.size();

	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;
	//1. Vertex Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexColor) * vertexCount;
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
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &index[0];

		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}

bool Objects::BoundingFrustum::Contain(Objects::BoundingBox * box)
{
	bool flag = false;

	for (int i = 0; i < 6; i++)
	{
		bool c = box->Intersects(plane[i]);

		if (c == false)
			return false;
		if (c == true)
			flag = true;
	}

	if (flag == false)
		return false;

	return true;
}

bool Objects::BoundingFrustum::Contain(Objects::BoundingSphere * sphere)
{
	D3DXVECTOR3 center = sphere->GetCenter();
	float rad = sphere->GetRadius();

	int num = 0;
	for (int i = 0; i < 6; i++)
	{
		Objects::Plane* p = plane[i];
		float nor = p->Normal.x * center.x + p->Normal.y * center.y + p->Normal.z * center.z;
		float d = nor + p->D;

		if (d > rad)
			return false;
		if (d < -rad) num++;
	}

	return true;
}
