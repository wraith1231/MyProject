#include "stdafx.h"
#include "ModelCapsule.h"

ModelCapsule::ModelCapsule(D3DXVECTOR3 maxVer, D3DXVECTOR3 minVer)
{
	float x = (maxVer.x + minVer.x) / 2;
	float z = (maxVer.z + minVer.z) / 2;

	float temp1 = maxVer.x - x;
	float temp2 = maxVer.z - z;

	if (fabs(temp1) < fabs(temp2))
		radius = fabs(temp1);
	else
		radius = fabs(temp2);

	float y1 = maxVer.y - radius;
	float y2 = minVer.y + radius;

	if (y1 < y2)
	{
		float temp = y1;
		y1 = y2;
		y2 = temp;
	}
	center = D3DXVECTOR3(x, (y1 + y2) / 2, z);
	height = fabs(y1 - y2);

	//À§ÂÊ Ä¸½¶
	{
		float rad = (float)D3DX_PI;
		for (size_t i = 0; i < 25; i++)
		{
			VertexColor ver;
			ver.color = D3DXCOLOR(0, 1.0f, 0, 1.0f);
			ver.position = D3DXVECTOR3(center.x + (radius) * cosf(rad), y1 + (radius) * -sinf(rad), center.z);
			vertices.push_back(ver);
			rad += static_cast<float>((D3DX_PI) / 25);
		}
		for (size_t i = 0; i < 25; i++)
		{
			indices.push_back(i);
			if (i == 24) indices.push_back(i - 1);
			else indices.push_back(i+1);
		}
	}
	{
		float rad = 0.0f;
		for (size_t i = 0; i < 50; i++)
		{
			VertexColor ver;
			ver.color = D3DXCOLOR(0, 1.0f, 0, 1.0f);
			ver.position = D3DXVECTOR3(center.x + (radius) * cosf(rad), y1, center.z + (radius) * sinf(rad));
			vertices.push_back(ver);
			rad += static_cast<float>((D3DX_PI*2) / 50);
		}
		for (size_t i = 0; i < 50; i++)
		{
			indices.push_back(i);
			if (i == 49) indices.push_back(0);
			else indices.push_back(i + 1);
		}
	}
	//¾Æ·¡ÂÊ Ä¸½¶
	{
		float rad = (float)D3DX_PI;
		for (size_t i = 0; i < 25; i++)
		{
			VertexColor ver;
			ver.color = D3DXCOLOR(0, 1.0f, 0, 1.0f);
			ver.position = D3DXVECTOR3(center.x + (radius) * cosf(rad), y2 - (radius) * -sinf(rad), center.z);
			vertices.push_back(ver);
			rad += static_cast<float>((D3DX_PI) / 25);
		}
		for (size_t i = 0; i < 25; i++)
		{
			indices.push_back(i);
			if (i == 24) indices.push_back(i - 1);
			else indices.push_back(i + 1);
		}
	}
	{
		float rad = 0.0f;
		for (size_t i = 0; i < 50; i++)
		{
			VertexColor ver;
			ver.color = D3DXCOLOR(0, 1.0f, 0, 1.0f);
			ver.position = D3DXVECTOR3(center.x + (radius) * cosf(rad), y2, center.z + (radius) * sinf(rad));
			vertices.push_back(ver);
			rad += static_cast<float>((D3DX_PI * 2) / 50);
		}
		for (size_t i = 0; i < 50; i++)
		{
			indices.push_back(i);
			if (i == 49) indices.push_back(0);
			else indices.push_back(i + 1);
		}
	}
	indices.push_back(0); indices.push_back(75);
	indices.push_back(24); indices.push_back(99);

	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;
	//1. Vertex Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexColor) * vertices.size();
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

	D3DXMatrixIdentity(&world);
	D3DXMatrixIdentity(&absoluteTransform);
	material = new Material();
	material->SetShader(Shaders + L"040_Objects.hlsl");
	worldBuffer = new WorldBuffer();

	direction = D3DXVECTOR3(0, 1, 0);
	scale = 1.0f;
}

ModelCapsule::~ModelCapsule()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

void ModelCapsule::Render()
{
	//À§ÂÊ Ä¸½¶
	material->SetBuffer();
	worldBuffer->SetVSBuffer(1);
	UINT stride = sizeof(VertexColor);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	D3D::GetDC()->DrawIndexed(50, 0, 0);
	
	//À§ÂÊÄ¸½¶ ¿ø
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	D3D::GetDC()->DrawIndexed(100, 50, 25);

	//¾Æ·§ÂÊÄ¸½¶
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	D3D::GetDC()->DrawIndexed(50, 150, 75);

	//¾Æ·§ÂÊÄ¸½¶ ¿ø
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	D3D::GetDC()->DrawIndexed(100, 200, 100);

	//¾Æ·§ÂÊÄ¸½¶ ¿ø
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	D3D::GetDC()->DrawIndexed(4, 300, 0);
}

void ModelCapsule::SetWorld(D3DXMATRIX mat)
{
	D3DXMATRIX temp = absoluteTransform * mat;
	worldBuffer->SetMatrix(temp);
	world = temp;

	D3DXVec3TransformCoord(&center, &center, &temp);
	D3DXVec3TransformNormal(&direction, &direction, &temp);
}

float ModelCapsule::GetVectorAngle(D3DXVECTOR3 & v1, D3DXVECTOR3 & v2)
{
	float vDot = D3DXVec3Dot(&v1, &v2);
	float len1 = D3DXVec3Length(&v1);
	float len2 = D3DXVec3Length(&v2);

	float angle = acosf(vDot / (len1 * len2));

	return angle;
}

bool ModelCapsule::LineCheck(D3DXVECTOR3 & l1, D3DXVECTOR3 & l2, float len)
{
	if (GetVectorAngle(l1, l2) < D3DX_PI / 2)
	{
		float vDot = D3DXVec3Dot(&l1, &l2);

		if (vDot < len*len)
			return true;

	}

	return false;
}

bool ModelCapsule::CapsuleCollision(ModelCapsule * capsule)
{
	//D3DXVECTOR3 cv1, cv2;
	//D3DXVECTOR3	c1, c2;

	//cv1 = D3DXVECTOR3(capsule->center.x, capsule->center.y + capsule->height / 2, capsule->center.z);
	//cv2 = D3DXVECTOR3(capsule->center.x, capsule->center.y - capsule->height / 2, capsule->center.z);

	//c1 = D3DXVECTOR3(center.x, center.y + height / 2, center.z);
	//c2 = D3DXVECTOR3(center.x, center.y - height / 2, center.z);
	//D3DXVec3TransformCoord(&c1, &c1, &world);
	//D3DXVec3TransformCoord(&c2, &c2, &world);

	//D3DXVec3TransformCoord(&cv1, &cv1, &world);
	//D3DXVec3TransformCoord(&cv2, &cv2, &world);

	////Ä¸½¶ À­ºÎºÐ ¿ø³¢¸® ºñ±³
	//float xdis = cv1.x - c1.x; xdis *= xdis;
	//float ydis = cv1.y - c1.y; ydis *= ydis;
	//float zdis = cv1.z - c1.z; zdis *= zdis;

	//float dist = sqrt(xdis + ydis + zdis);
	//if (dist < radius * scale || dist < capsule->radius * capsule->scale)
	//	return true;

	////Ä¸½¶ À­ºÎºÐ ¿ø°ú Ã¼Å©ÇÏ´Â ÂÊ ¾Æ·¡ÂÊ ¹Ý¿ø°ú ºñ±³
	//xdis = cv2.x - c1.x; xdis *= xdis;
	//ydis = cv2.y - c1.y; ydis *= ydis;
	//zdis = cv2.z - c1.z; zdis *= zdis;

	//dist = sqrt(xdis + ydis + zdis);
	//if (dist < radius * scale || dist < capsule->radius * capsule->scale)
	//	return true;

	////Ä¸½¶ ¾Æ·§ºÎºÐ ¿ø°ú Ã¼Å©ÇÏ´Â ÂÊ À­ÂÊ ¹Ý¿ø°ú ºñ±³
	//xdis = cv1.x - c2.x; xdis *= xdis;
	//ydis = cv1.y - c2.y; ydis *= ydis;
	//zdis = cv1.z - c2.z; zdis *= zdis;

	//dist = sqrt(xdis + ydis + zdis);
	//if (dist < radius * scale || dist < capsule->radius * capsule->scale)
	//	return true;

	////Ä¸½¶ ¾Æ·§ºÎºÐ ¿ø°ú Ã¼Å©ÇÏ´Â ÂÊ ¾Æ·¡ÂÊ ¹Ý¿ø°ú ºñ±³
	//xdis = cv2.x - c2.x; xdis *= xdis;
	//ydis = cv2.y - c2.y; ydis *= ydis;
	//zdis = cv2.z - c2.z; zdis *= zdis;

	//dist = sqrt(xdis + ydis + zdis);
	//if (dist < radius * scale || dist < capsule->radius * capsule->scale)
	//	return true;

	D3DXVECTOR3 cv1, cv2, c1, c2;

	c1 = center - direction * (height / 2);
	c2 = center + direction * (height / 2);

	cv1 = capsule->center - capsule->direction * (capsule->height / 2);
	cv2 = capsule->center + capsule->direction * (capsule->height / 2);

	if (direction == capsule->direction || direction == -capsule->direction)
	{
		D3DXVECTOR3 temp, temp1;
		temp1 = c1 - cv1;
		D3DXVec3Cross(&temp, &temp1, &capsule->direction);
		float dis = fabs(D3DXVec3Length(&temp) / D3DXVec3Length(&capsule->direction));

		if (dis < radius + capsule->radius)
		{
			D3DXVECTOR3 v1 = c2 - c1;
			D3DXVECTOR3 v2 = cv2 - cv1;

			D3DXVECTOR3 v12 = c1 - v2;
			D3DXVECTOR3 v22 = c2 - v2;
			D3DXVECTOR3 v11 = cv1 - v2;
			D3DXVECTOR3 v21 = cv2 - v2;

			float len1 = D3DXVec3Length(&v1);
			float len2 = D3DXVec3Length(&v2);

			if (LineCheck(v1, v11, len1) == true) return true;
			if (LineCheck(v1, v21, len1) == true) return true;
			if (LineCheck(v2, v12, len1) == true) return true;
			if (LineCheck(v2, v22, len1) == true) return true;
		}
		else
			return false;
	}
	else
	{
		float s1, s2;

		D3DXVECTOR3 w0 = c1 - cv1;
		D3DXVECTOR3 wc = w0 + direction - capsule->direction;

		float f1, f2, f3, f4, f5;

		f1 = D3DXVec3Dot(&direction, &direction);
		f2 = D3DXVec3Dot(&direction, &capsule->direction);
		f3 = D3DXVec3Dot(&capsule->direction, &capsule->direction);
		f4 = D3DXVec3Dot(&direction, &w0);
		f5 = D3DXVec3Dot(&capsule->direction, &w0);

		s1 = (f2 * f5 - f3 * f4) / (f1 * f3 - f2 * f2);
		s2 = (f2 * f5 - f3 * f4) / (f1 * f3 - f2 * f2);

		float dis = 99999.0f;
		if (s1 < 0)
		{
			D3DXVECTOR3 temp, temp1;
			temp1 = c1 - cv1;
			D3DXVec3Cross(&temp, &temp1, &capsule->direction);
			dis = fabs(D3DXVec3Length(&temp) / D3DXVec3Length(&capsule->direction));
		}
		else if (s1 < 0)
		{
			D3DXVECTOR3 temp, temp1;
			temp1 = c2 - cv1;
			D3DXVec3Cross(&temp, &temp1, &capsule->direction);
			dis = fabs(D3DXVec3Length(&temp) / D3DXVec3Length(&capsule->direction));
		}
		else if (s1 < 0)
		{
			D3DXVECTOR3 temp, temp1;
			temp1 = cv1 - c1;
			D3DXVec3Cross(&temp, &temp1, &direction);
			dis = fabs(D3DXVec3Length(&temp) / D3DXVec3Length(&direction));
		}
		else if (s1 < 0)
		{
			D3DXVECTOR3 temp, temp1;
			temp1 = cv2 - c1;
			D3DXVec3Cross(&temp, &temp1, &direction);
			dis = fabs(D3DXVec3Length(&temp) / D3DXVec3Length(&direction));
		}
		else
		{
			D3DXVECTOR3 v1 = c1 + direction * s1;
			D3DXVECTOR3 v2 = cv1 + capsule->direction * s2;

			D3DXVECTOR3 temp = v1 - v2;
			float len = D3DXVec3Length(&temp);

			if (len < radius + capsule->radius) return true;
		}

		if (dis < radius + capsule->radius) return true;
	}

	return false;
}
