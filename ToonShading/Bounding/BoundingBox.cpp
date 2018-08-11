#include "stdafx.h"
#include "BoundingBox.h"

#include "ObjectsPlane.h"
#include "ObjectsRay.h"

Objects::BoundingBox::BoundingBox(D3DXVECTOR3 max, D3DXVECTOR3 min)
{
	worldBuffer = new WorldBuffer;
	this->max = max;
	this->min = min;
	oMax = max;
	oMin = min;

	shader = new Shader(Shaders + L"040_Objects.hlsl");
	buffer = new BoxBuffer();

	vertex.resize(8);
	for (size_t i = 0; i < 8; i++)
		vertex[i].color = D3DXCOLOR(0, 0, 0, 1);
	vertex[0].position = D3DXVECTOR3(min.x, min.y, min.z);
	vertex[1].position = D3DXVECTOR3(min.x, min.y, max.z);
	vertex[2].position = D3DXVECTOR3(max.x, min.y, max.z);
	vertex[3].position = D3DXVECTOR3(max.x, min.y, min.z);
	
	vertex[4].position = D3DXVECTOR3(min.x, max.y, min.z);
	vertex[5].position = D3DXVECTOR3(min.x, max.y, max.z);
	vertex[6].position = D3DXVECTOR3(max.x, max.y, max.z);
	vertex[7].position = D3DXVECTOR3(max.x, max.y, min.z);

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

Objects::BoundingBox::~BoundingBox()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(buffer);
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

void Objects::BoundingBox::CreateFromPoints(vector<D3DXVECTOR3> points, Objects::BoundingBox** box)
{
	if (points.size() < 1)
	{
		return;
	}

	bool flag = false;
	D3DXVECTOR3 vector3 = D3DXVECTOR3(100000, 100000, 100000);
	D3DXVECTOR3 vector31 = D3DXVECTOR3(-100000, -100000, -100000);

	for (D3DXVECTOR3 point : points)
	{
		D3DXVECTOR3 vector32 = point;
		Objects::BoundingBox::VectorMin(vector3, vector32, vector3);
		Objects::BoundingBox::VectorMax(vector31, vector32, vector31);
		flag = true;
	}

	if (flag == false)
		return;

	*box = new BoundingBox(vector3, vector31);
}

void Objects::BoundingBox::CreateFromPoints(D3DXVECTOR3 * points, Objects::BoundingBox ** box)
{
	if (points == NULL)
		return;

	bool flag = false;
	D3DXVECTOR3 vector3 = D3DXVECTOR3(100000, 100000, 100000);
	D3DXVECTOR3 vector31 = D3DXVECTOR3(-100000, -100000, -100000);

	for (size_t i = 0; i < _msize(points) / sizeof(D3DXVECTOR3); i++)
	{
		D3DXVECTOR3 vector32 = points[i];
		Objects::BoundingBox::VectorMin(vector3, vector32, vector3);
		Objects::BoundingBox::VectorMax(vector31, vector32, vector31);
		flag = true;
	}

	if (flag == false)
		return;
	
	*box = new BoundingBox(vector3, vector31);
}

void Objects::BoundingBox::VectorMin(D3DXVECTOR3 & v1, D3DXVECTOR3 & v2, OUT D3DXVECTOR3 & v3)
{
	v3.x = v1.x > v2.x ? v2.x : v1.x;
	v3.y = v1.y > v2.y ? v2.y : v1.y;
	v3.z = v1.z > v2.z ? v2.z : v1.z;
}

void Objects::BoundingBox::VectorMax(D3DXVECTOR3 & v1, D3DXVECTOR3 & v2, OUT D3DXVECTOR3 & v3)
{
	v3.x = v1.x > v2.x ? v1.x : v2.x;
	v3.y = v1.y > v2.y ? v1.y : v2.y;
	v3.z = v1.z > v2.z ? v1.z : v2.z;
}

void Objects::BoundingBox::Update()
{
}

void Objects::BoundingBox::Update(D3DXMATRIX world)
{
	D3DXVec3TransformCoord(&max, &oMax, &world);
	D3DXVec3TransformCoord(&min, &oMin, &world);

	worldBuffer->SetMatrix(world);
}

void Objects::BoundingBox::Update(D3DXVECTOR3 pos)
{
	max = oMax + pos;
	min = oMin + pos;

	D3DXMATRIX mat = worldBuffer->GetMatrix();
	mat._41 = pos.x;
	mat._42 = pos.y;
	mat._43 = pos.z;

	worldBuffer->SetMatrix(mat);
}

void Objects::BoundingBox::Render()
{
	buffer->SetPSBuffer(2);

	//D3DXMATRIX world;
	//D3DXMatrixIdentity(&world);

	UINT stride = sizeof(VertexColor);
	UINT offset = 0;

	worldBuffer->SetVSBuffer(1);
	shader->Render();

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	D3D::GetDC()->DrawIndexed(index.size(), 0, 0);
}

void Objects::BoundingBox::Render(D3DXMATRIX mat)
{
	worldBuffer->SetMatrix(mat);
	Render();
}

bool Objects::BoundingBox::Intersects(Objects::BoundingBox * box)
{
	if (this->max.x < box->min.x || this->min.x > box->max.x)
		return false;
	if (this->max.y < box->min.y || this->min.y > box->max.y)
		return false;
	if (this->max.z < box->min.z || this->min.z > box->max.z)
		return false;

	return true;
}

bool Objects::BoundingBox::Intersects(Objects::Plane * plane)
{
	D3DXVECTOR3 vec3 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vec31 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	vec31.x = plane->Normal.x >= 0.0f ? min.x : max.x;
	vec31.y = plane->Normal.y >= 0.0f ? min.y : max.y;
	vec31.z = plane->Normal.z >= 0.0f ? min.z : max.z;

	vec3.x = plane->Normal.x >= 0.0f ? max.x : min.x;
	vec3.y = plane->Normal.y >= 0.0f ? max.y : min.y;
	vec3.z = plane->Normal.z >= 0.0f ? max.z : min.z;

	if (plane->Normal.x * vec31.x + plane->Normal.y * vec31.y + plane->Normal.z * vec31.z + plane->D > 0.01f)
		return false;
	if (plane->Normal.x * vec3.x + plane->Normal.y * vec3.y + plane->Normal.z * vec3.z + plane->D < -0.01f)
		return false;

	return true;
}

bool Objects::BoundingBox::Intersects(D3DXPLANE plane)
{
	D3DXVECTOR3 vec3 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vec31 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	vec31.x = plane.a >= 0.0f ? min.x : max.x;
	vec31.y = plane.b >= 0.0f ? min.y : max.y;
	vec31.z = plane.c >= 0.0f ? min.z : max.z;

	vec3.x = plane.a >= 0.0f ? max.x : min.x;
	vec3.y = plane.b >= 0.0f ? max.y : min.y;
	vec3.z = plane.c >= 0.0f ? max.z : min.z;

	if (plane.a * vec31.x + plane.b * vec31.y + plane.c * vec31.z + plane.d > 0.01f)
		return false;
	if (plane.a * vec3.x + plane.b * vec3.y + plane.c * vec3.z + plane.d < -0.01f)
		return false;

	return true;
}

bool Objects::BoundingBox::Intersects(Objects::Ray * ray)
{
	float single = 0.0f;
	float single1 = 999999.0f;
	if (fabs(ray->Direction.x) >= 1E-06f)
	{
		float x = 1.0f / ray->Direction.x;
		float x1 = (this->min.x - ray->Position.x) * x;
		float x2 = (this->max.x - ray->Position.x) * x;

		if (x1 > x2)
		{
			float single2 = x1;
			x1 = x2;
			x2 = single2;
		}
		single = x1 > single ? x1 : single;
		single1 = x2 < single1 ? x2 : single;
		if (single > single1)
		{
			return false;
		}
	}
	else if (ray->Position.x < this->min.x || ray->Position.x > this->max.x)
		return false;

	if (fabs(ray->Direction.z) >= 1E-06f)
	{
		float z = 1.0f / ray->Direction.z;
		float z1 = (this->min.z - ray->Position.z) * z;
		float z2 = (this->max.z - ray->Position.z) * z;
		if (z1 > z2)
		{
			float single4 = z1;
			z1 = z2;
			z2 = single4;
		}

		single = z1 > single ? z1 : single;
		single1 = z2 < single1 ? z2 : single1;
		if (single > single1)
			return false;
	}
	else if (ray->Position.z < this->min.z || ray->Position.z > this->max.z)
		return false;

	
	return true;
}
