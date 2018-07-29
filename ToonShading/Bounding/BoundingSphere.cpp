#include "stdafx.h"
#include "BoundingSphere.h"

#include "BoundingBox.h"

Objects::BoundingSphere::BoundingSphere(D3DXVECTOR3 center, float radius)
	: vertexBuffer(NULL), indexBuffer(NULL)
{
	worldBuffer = new WorldBuffer;
	buffer = new SphBuffer();
	shader = new Shader(Shaders + L"040_Objects.hlsl");

	if (radius < 0.0f)
		radius = 5.0f;

	this->center = center;
	this->radius = radius;
}

Objects::BoundingSphere::BoundingSphere(D3DXVECTOR3 center, D3DXVECTOR3 max, D3DXVECTOR3 min)
	: vertexBuffer(NULL), indexBuffer(NULL)
{
	worldBuffer = new WorldBuffer;
	buffer = new SphBuffer();
	shader = new Shader(Shaders + L"040_Objects.hlsl");

	float x = max.x - min.x;
	float y = max.y - min.y;
	float z = max.z - min.z;
	x /= 2;
	y /= 2;
	z /= 2;

	float rad = x > y ? x : y;
	rad = rad > z ? rad : z;

	if (radius < 0.0f)
		radius = 5.0f;

	this->center = center;
	this->radius = radius;

}

Objects::BoundingSphere::~BoundingSphere()
{
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(buffer);
	SAFE_DELETE(shader);

	if(vertexBuffer != NULL)
		SAFE_RELEASE(vertexBuffer);
	if (indexBuffer != NULL)
		SAFE_RELEASE(indexBuffer);
}

Objects::BoundingSphere * Objects::BoundingSphere::CreateFromBoundingBox(Objects::BoundingBox* box)
{
	float single;
	Objects::BoundingSphere* sphere = new Objects::BoundingSphere();
	Math::GetLerp(box->GetMin(), box->GetMax(), 0.5f, sphere->center);
	Math::GetDistance(box->GetMin(), box->GetMax(), single);
	sphere->radius = single * 0.5f;
	return sphere;
}

void Objects::BoundingSphere::CreateFromBoundingBox(Objects::BoundingBox* box, BoundingSphere ** sphere)
{
	*sphere = new Objects::BoundingSphere();
	float single;
	Math::GetLerp(box->GetMin(), box->GetMax(), 0.5f, (*sphere)->center);
	Math::GetDistance(box->GetMin(), box->GetMax(), single);
	(*sphere)->radius = single * 0.5f;
}

Objects::BoundingSphere * Objects::BoundingSphere::CreateFromPoint(vector<D3DXVECTOR3> points)
{
	float single;
	float single1;
	float single2;
	float single3;
	D3DXVECTOR3 vector3;

	Objects::BoundingSphere* ret = new Objects::BoundingSphere();
	D3DXVECTOR3 x;
	if (points.size() < 1)
		return NULL;

	D3DXVECTOR3 vector31 = points[0];
	D3DXVECTOR3 vector32 = points[0];
	D3DXVECTOR3 vector33 = points[0];
	D3DXVECTOR3 vector34 = points[0];
	D3DXVECTOR3 vector35 = points[0];
	D3DXVECTOR3 vector36 = points[0];

	for (D3DXVECTOR3 point : points)
	{
		if (point.x < vector36.x)
			vector36 = point;
		if (point.x > vector35.x)
			vector35 = point;
		if (point.y < vector34.y)
			vector34 = point;
		if (point.y > vector33.y)
			vector33 = point;
		if (point.z < vector32.z)
			vector32 = point;
		if (point.z >= vector31.z)
			continue;

		vector31 = point;
	}

	Math::GetDistance(vector35, vector36, single3);
	Math::GetDistance(vector33, vector34, single2);
	Math::GetDistance(vector31, vector32, single1);

	if (single3 > single2)
	{
		if (single3 <= single1)
		{
			Math::GetLerp(vector31, vector32, 0.5f, vector3);
			single = single1 * 0.5f;
		}
		else
		{
			Math::GetLerp(vector35, vector36, 0.5f, vector3);
			single = single3 * 0.5f;
		}
	}
	else if (single2 <= single1)
	{
		Math::GetLerp(vector31, vector32, 0.5f, vector3);
		single = single1 * 0.5f;
	}
	else
	{
		Math::GetLerp(vector33, vector34, 0.5f, vector3);
		single = single2 * 0.5f;
	}

	for (D3DXVECTOR3 point1 : points)
	{
		x.x = point1.x - vector3.x;
		x.y = point1.y - vector3.y;
		x.z = point1.z - vector3.z;
		float single4 = D3DXVec3Length(&x);
		if (single4 <= single)
			continue;
		single = (single + single4) * 0.5f;
		vector3 = vector3 + ((1.0f - single / single4) * x);
	}
	ret->center = vector3;
	ret->radius = single;

	return ret;
}

Objects::BoundingSphere * Objects::BoundingSphere::CreateFromPoint(D3DXVECTOR3 * points)
{
	float single;
	float single1;
	float single2;
	float single3;
	D3DXVECTOR3 vector3;

	Objects::BoundingSphere* ret = new Objects::BoundingSphere();
	D3DXVECTOR3 x;
	if (points == NULL)
		return NULL;

	D3DXVECTOR3 vector31 = points[0];
	D3DXVECTOR3 vector32 = points[0];
	D3DXVECTOR3 vector33 = points[0];
	D3DXVECTOR3 vector34 = points[0];
	D3DXVECTOR3 vector35 = points[0];
	D3DXVECTOR3 vector36 = points[0];

	for (UINT i = 0; i < _msize(points) / sizeof(D3DXVECTOR3); i++)
	{
		if (points[i].x < vector36.x)
			vector36 = points[i];
		if (points[i].x > vector35.x)
			vector35 = points[i];
		if (points[i].y < vector34.y)
			vector34 = points[i];
		if (points[i].y > vector33.y)
			vector33 = points[i];
		if (points[i].z < vector32.z)
			vector32 = points[i];
		if (points[i].z >= vector31.z)
			continue;

		vector31 = points[i];
	}

	Math::GetDistance(vector35, vector36, single3);
	Math::GetDistance(vector33, vector34, single2);
	Math::GetDistance(vector31, vector32, single1);

	if (single3 > single2)
	{
		if (single3 <= single1)
		{
			Math::GetLerp(vector31, vector32, 0.5f, vector3);
			single = single1 * 0.5f;
		}
		else
		{
			Math::GetLerp(vector35, vector36, 0.5f, vector3);
			single = single3 * 0.5f;
		}
	}
	else if (single2 <= single1)
	{
		Math::GetLerp(vector31, vector32, 0.5f, vector3);
		single = single1 * 0.5f;
	}
	else
	{
		Math::GetLerp(vector33, vector34, 0.5f, vector3);
		single = single2 * 0.5f;
	}

	for (UINT i = 0; i < _msize(points) / sizeof(D3DXVECTOR3) ;i++)
	{
		x.x = points[i].x - vector3.x;
		x.y = points[i].y - vector3.y;
		x.z = points[i].z - vector3.z;
		float single4 = D3DXVec3Length(&x);
		if (single4 <= single)
			continue;
		single = (single + single4) * 0.5f;
		vector3 = vector3 + ((1.0f - single / single4) * x);
	}
	ret->center = vector3;
	ret->radius = single;

	return ret;
}

void Objects::BoundingSphere::SetVertex(UINT num)
{

	//Vertex & Index Setting
	if (num == 0)
	{
		float rad = 0.0f;
		for (int i = 0; i <= 50; i++)
		{
			VertexColor ver;
			ver.color = D3DXCOLOR(0, 0, 0, 0);
			ver.position = D3DXVECTOR3(center.x, (center.y + radius) * cosf(rad), (center.z + radius) * -sinf(rad));
			vertex.push_back(ver);
			rad += static_cast<float>((D3DX_PI * 2) / 50);
		}
		for (int i = 0; i <= 50; i++)
		{
			index.push_back(i);
			index.push_back(i + 1);
		}
	}
	else if (num == 1)
	{
		//y 중심
		float rad = 0.0f;
		for (int i = 0; i <= 50; i++)
		{
			VertexColor ver;
			ver.color = D3DXCOLOR(0, 0, 0, 0);
			ver.position = D3DXVECTOR3((center.x + radius) * cosf(rad), center.y, (center.z + radius) * sinf(rad));
			vertex.push_back(ver);
			rad += static_cast<float>((D3DX_PI * 2) / 50);
		}
		for (int i = 0; i <= 50; i++)
		{
			index.push_back(i);
			index.push_back(i + 1);
		}
	}
	else if (num == 2)
	{
		//z 중심
		float rad = 0.0f;
		for (int i = 0; i <= 50; i++)
		{
			VertexColor ver;
			ver.color = D3DXCOLOR(0, 0, 0, 0);
			ver.position = D3DXVECTOR3((center.x + radius) * cosf(rad), (center.y + radius) * -sinf(rad), center.z);
			vertex.push_back(ver);
			rad += static_cast<float>((D3DX_PI * 2) / 50);
		}
		for (int i = 0; i <= 50; i++)
		{
			index.push_back(i);
			index.push_back(i + 1);
		}
	}
	else if (num == 3)
	{

		float rad = 0.0f;
		for (int i = 0; i <= 50; i++)
		{
			VertexColor ver;
			ver.color = D3DXCOLOR(0, 0, 0, 0);
			ver.position = D3DXVECTOR3(center.x, (center.y + radius) * cosf(rad), (center.z + radius) * -sinf(rad));
			vertex.push_back(ver);
			rad += static_cast<float>((D3DX_PI * 2) / 50);
		}
		rad = 0.0f;
		for (int i = 0; i <= 50; i++)
		{
			VertexColor ver;
			ver.color = D3DXCOLOR(0, 0, 0, 0);
			ver.position = D3DXVECTOR3((center.x + radius) * cosf(rad), center.y, (center.z + radius) * sinf(rad));
			vertex.push_back(ver);
			rad += static_cast<float>((D3DX_PI * 2) / 50);
		}
		rad = 0.0f;
		for (int i = 0; i <= 50; i++)
		{
			VertexColor ver;
			ver.color = D3DXCOLOR(0, 0, 0, 0);
			ver.position = D3DXVECTOR3((center.x + radius) * cosf(rad), (center.y + radius) * -sinf(rad), center.z);
			vertex.push_back(ver);
			rad += static_cast<float>((D3DX_PI * 2) / 50);
		}
		for (UINT i = 0; i < vertex.size(); i++)
		{
			index.push_back(i);
			index.push_back(i + 1);
		}
	}
	else
	{
		return;
	}


	position = center;

	//Buffer Setting
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

void Objects::BoundingSphere::Update()
{
}

void Objects::BoundingSphere::Render()
{
	buffer->SetPSBuffer(2);

	//D3DXMATRIX world;
	//D3DXMatrixTranslation(&world, center.x, center.y, center.z);
	//
	//worldBuffer->SetMatrix(world);

	UINT stride = sizeof(VertexColor);
	UINT offset = 0;
	
	worldBuffer->SetVSBuffer(1);
	shader->Render();
	
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	D3D::GetDC()->DrawIndexed(index.size(), 0, 0);
}

void Objects::BoundingSphere::Render(D3DXMATRIX mat)
{
	worldBuffer->SetMatrix(mat);
	Render();
}

bool Objects::BoundingSphere::Intersects(Objects::BoundingBox * box)
{
	float single;
	D3DXVECTOR3 vec(0, 0, 0);
	vec = Math::Clamp(this->center, box->GetMin(), box->GetMax());

	//center, vec, single

	float x = this->center.x - vec.x;
	float y = this->center.y - vec.y;
	float z = this->center.z - vec.z;

	single = x * x + y * y + z * z;

	if (single <= this->radius * this->radius)
		return true;

	return false;
}