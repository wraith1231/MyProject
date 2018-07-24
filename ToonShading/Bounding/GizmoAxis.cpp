#include "stdafx.h"
#include "GizmoAxis.h"

GizmoAxis::GizmoAxis(D3DXVECTOR3 position)
{
	worldBuffer = new WorldBuffer();
	material = new Material();// Shaders + L"040_Objects.hlsl");
	material->SetShader(Shaders + L"040_Objects.hlsl");
	xSelected = ySelected = zSelected = false;
	mode = 0;
	D3DXMatrixIdentity(&tempWorld);

	s = D3DXVECTOR3(1, 1, 1);
	r = t = D3DXVECTOR3(0, 0, 0);

	GizmoVertexType ver;
	//X
	ver.color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

	ver.position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	lineVertex.push_back(ver);

	ver.position += D3DXVECTOR3(2.0f, 0.0f, 0.0f);
	lineVertex.push_back(ver);
	
	//Y
	ver.color = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);

	ver.position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	lineVertex.push_back(ver);

	ver.position += D3DXVECTOR3(0.0f, 2.0f, 0.0f);
	lineVertex.push_back(ver);

	//Z
	ver.color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);

	ver.position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	lineVertex.push_back(ver);

	ver.position += D3DXVECTOR3(0.0f, 0.0f, 2.0f);
	lineVertex.push_back(ver);

	lineIndex.push_back(0); lineIndex.push_back(1);
	lineIndex.push_back(2); lineIndex.push_back(3);
	lineIndex.push_back(4); lineIndex.push_back(5);

	//Line Click Box
	D3DXVECTOR3 ve;
	ve = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//X
	ve = D3DXVECTOR3(0.0f, -0.1f, 0.1f);
	xAxisVer.push_back(ve);
	ve = D3DXVECTOR3(0.0f, -0.1f, -0.1f);
	xAxisVer.push_back(ve);
	ve = D3DXVECTOR3(1.0f, -0.1f, -0.1f);
	xAxisVer.push_back(ve);
	ve = D3DXVECTOR3(1.0f, -0.1f, 0.1f);
	xAxisVer.push_back(ve);

	ve = D3DXVECTOR3(0.0f, 0.1f, 0.1f);
	xAxisVer.push_back(ve);
	ve = D3DXVECTOR3(0.0f, 0.1f, -0.1f);
	xAxisVer.push_back(ve);
	ve = D3DXVECTOR3(1.0f, 0.1f, -0.1f);
	xAxisVer.push_back(ve);
	ve = D3DXVECTOR3(1.0f, 0.1f, 0.1f);
	xAxisVer.push_back(ve);

	//Y
	ve = D3DXVECTOR3(0.1f, 0.0f, 0.1f);
	yAxisVer.push_back(ve);
	ve = D3DXVECTOR3(0.1f, 0.0f, -0.1f);
	yAxisVer.push_back(ve);
	ve = D3DXVECTOR3(-0.1f, 0.0f, -0.1f);
	yAxisVer.push_back(ve);
	ve = D3DXVECTOR3(-0.1f, 0.0f, 0.1f);
	yAxisVer.push_back(ve);

	ve = D3DXVECTOR3(0.1f, 1.0f, 0.1f);
	yAxisVer.push_back(ve);
	ve = D3DXVECTOR3(0.1f, 1.0f, -0.1f);
	yAxisVer.push_back(ve);
	ve = D3DXVECTOR3(-0.1f, 1.0f, -0.1f);
	yAxisVer.push_back(ve);
	ve = D3DXVECTOR3(-0.1f, 1.0f, 0.1f);
	yAxisVer.push_back(ve);

	//Z
	ve = D3DXVECTOR3(0.1f, -0.1f, 1.0f);
	zAxisVer.push_back(ve);
	ve = D3DXVECTOR3(0.1f, -0.1f, 0.0f);
	zAxisVer.push_back(ve);
	ve = D3DXVECTOR3(-0.1f, -0.1f, 0.0f);
	zAxisVer.push_back(ve);
	ve = D3DXVECTOR3(-0.1f, -0.1f, 1.0f);
	zAxisVer.push_back(ve);

	ve = D3DXVECTOR3(0.1f, 0.1f, 1.0f);
	zAxisVer.push_back(ve);
	ve = D3DXVECTOR3(0.1f, 0.1f, 0.0f);
	zAxisVer.push_back(ve);
	ve = D3DXVECTOR3(-0.1f, 0.1f, 0.0f);
	zAxisVer.push_back(ve);
	ve = D3DXVECTOR3(-0.1f, 0.1f, 1.0f);
	zAxisVer.push_back(ve);

	axisIndex.push_back(2); axisIndex.push_back(6); axisIndex.push_back(1);
	axisIndex.push_back(1); axisIndex.push_back(6); axisIndex.push_back(5);

	axisIndex.push_back(0); axisIndex.push_back(4); axisIndex.push_back(3);
	axisIndex.push_back(3); axisIndex.push_back(4); axisIndex.push_back(7);

	axisIndex.push_back(3); axisIndex.push_back(7); axisIndex.push_back(2);
	axisIndex.push_back(2); axisIndex.push_back(7); axisIndex.push_back(6);

	axisIndex.push_back(1); axisIndex.push_back(5); axisIndex.push_back(0);
	axisIndex.push_back(0); axisIndex.push_back(5); axisIndex.push_back(4);

	axisIndex.push_back(6); axisIndex.push_back(7); axisIndex.push_back(5);
	axisIndex.push_back(5); axisIndex.push_back(7); axisIndex.push_back(4);

	axisIndex.push_back(3); axisIndex.push_back(2); axisIndex.push_back(0);
	axisIndex.push_back(0); axisIndex.push_back(2); axisIndex.push_back(1);


	D3DXMATRIX T;
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);
	worldBuffer->SetMatrix(T);

	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;
	//1. Vertex Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(GizmoVertexType) * lineVertex.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &lineVertex[0];

		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &lineVertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//2. Index Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * lineIndex.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &lineIndex[0];

		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &lineIndexBuffer);
		assert(SUCCEEDED(hr));
	}

	D3DXMatrixIdentity(&originTransform);

	//rasterize 
	{
		D3D11_RASTERIZER_DESC desc;
		States::GetRasterizerDesc(&desc);
		States::CreateRasterizer(&desc, &rasterize[0]);

		desc.FrontCounterClockwise = true;
		States::CreateRasterizer(&desc, &rasterize[1]);
	}

	//depth stencil
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		States::GetDepthStencilDesc(&desc);
		States::CreateDepthStencil(&desc, &depthStencilState[0]);

		desc.DepthEnable = false;	//깊이 관련된거
		States::CreateDepthStencil(&desc, &depthStencilState[1]);
	}
}

GizmoAxis::~GizmoAxis()
{
	SAFE_RELEASE(lineIndexBuffer);
	SAFE_RELEASE(lineVertexBuffer);
	SAFE_RELEASE(rasterize[0]);
	SAFE_RELEASE(rasterize[1]);

	SAFE_RELEASE(depthStencilState[0]);
	SAFE_RELEASE(depthStencilState[1]);
}

void GizmoAxis::Update()
{
	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, s.x, s.y, s.z);

	D3DXQUATERNION q;
	D3DXQuaternionRotationYawPitchRoll(&q, r.y, r.x, r.z);
	D3DXMatrixRotationQuaternion(&R, &q);

	D3DXMatrixTranslation(&T, t.x, t.y, t.z);

	worldBuffer->SetMatrix(originTransform * S * R * T);
	tempWorld = originTransform * S * R * T;
}

void GizmoAxis::Update(D3DXVECTOR3 position)
{
	D3DXMATRIX T;
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);
	worldBuffer->SetMatrix(T);
}

void GizmoAxis::Render()
{
	D3D::GetDC()->OMSetDepthStencilState(depthStencilState[1], 1);
	D3D::GetDC()->RSSetState(rasterize[1]);

	UINT stride = sizeof(GizmoVertexType);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &lineVertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(lineIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	D3DXMATRIX W = worldBuffer->GetMatrix();
	worldBuffer->SetMatrix(tempWorld);
	worldBuffer->SetVSBuffer(1);
	material->SetBuffer();

	D3D::GetDC()->DrawIndexed(lineIndex.size(), 0, 0);
	worldBuffer->SetMatrix(W);

	D3D::GetDC()->RSSetState(rasterize[0]);
	D3D::GetDC()->OMSetDepthStencilState(depthStencilState[0], 1);
}

bool GizmoAxis::MouseIntersect(D3DXVECTOR3 cam, D3DXVECTOR3 camDir, float& distance)
{
	//D3DXMATRIX world = worldBuffer->GetMatrix();
	xSelected = ySelected = zSelected = false;

	for (UINT i = 0; i < axisIndex.size(); i += 3)
	{
		D3DXVECTOR3 v1, v2, v3;
		v1 = xAxisVer[axisIndex[i + 0]];
		v2 = xAxisVer[axisIndex[i + 1]];
		v3 = xAxisVer[axisIndex[i + 2]];

		D3DXVec3TransformCoord(&v1, &v1, &tempWorld);
		D3DXVec3TransformCoord(&v2, &v2, &tempWorld);
		D3DXVec3TransformCoord(&v3, &v3, &tempWorld);

		float u, v;
		if (D3DXIntersectTri(&v1, &v2, &v3, &cam, &camDir, &u, &v, &distance) == TRUE)
		{
			xSelected = true;
			return true;
		}
	}
	for (UINT i = 0; i < axisIndex.size(); i += 3)
	{
		D3DXVECTOR3 v1, v2, v3;
		v1 = yAxisVer[axisIndex[i + 0]];
		v2 = yAxisVer[axisIndex[i + 1]];
		v3 = yAxisVer[axisIndex[i + 2]];

		D3DXVec3TransformCoord(&v1, &v1, &tempWorld);
		D3DXVec3TransformCoord(&v2, &v2, &tempWorld);
		D3DXVec3TransformCoord(&v3, &v3, &tempWorld);

		float u, v;
		if (D3DXIntersectTri(&v1, &v2, &v3, &cam, &camDir, &u, &v, &distance) == TRUE)
		{
			ySelected = true;
			return true;
		}
	}
	for (UINT i = 0; i < axisIndex.size(); i += 3)
	{
		D3DXVECTOR3 v1, v2, v3;
		v1 = zAxisVer[axisIndex[i + 0]];
		v2 = zAxisVer[axisIndex[i + 1]];
		v3 = zAxisVer[axisIndex[i + 2]];

		D3DXVec3TransformCoord(&v1, &v1, &tempWorld);
		D3DXVec3TransformCoord(&v2, &v2, &tempWorld);
		D3DXVec3TransformCoord(&v3, &v3, &tempWorld);

		float u, v;
		if (D3DXIntersectTri(&v1, &v2, &v3, &cam, &camDir, &u, &v, &distance) == TRUE)
		{
			zSelected = true;
			return true;
		}
	}

	return false;
}

void GizmoAxis::PositionChange(D3DXVECTOR3 change)
{
	if (xSelected == true)
	{
		if (mode == 0)
		{	//Translation
			t.x += change.x;
		}
		else if (mode == 1)
		{
			//Scaling
			s.x += change.x;
		}
		else if (mode == 2)
		{
			//Rotation
			r.x += change.x;
		}

		return;
	}
	if (ySelected == true)
	{
		if (mode == 0)
		{
			t.y += change.y;
		}
		else if (mode == 1)
		{
			//Scaling
			s.y += change.y;
		}
		else if (mode == 2)
		{
			//Rotation
			r.y += change.y;
		}
		return;
	}
	if (zSelected == true)
	{
		if (mode == 0)
		{
			t.z += change.z;
		}
		else if (mode == 1)
		{
			//Scaling
			s.z += change.z;
		}
		else if (mode == 2)
		{
			//Rotation
			r.z += change.z;
		}
		return;
	}

}

void GizmoAxis::SetWorld(D3DXMATRIX mat)
{
	worldBuffer->SetMatrix(mat);
	
	tempWorld = mat;
}

D3DXMATRIX GizmoAxis::MatrixInvert(D3DXMATRIX mat)
{
	D3DXMATRIX matrix1;
	float m11 = mat._11;
	float m12 = mat._12;
	float m13 = mat._13;
	float m14 = mat._14;
	float m21 = mat._21;
	float m22 = mat._22;
	float m23 = mat._23;
	float m24 = mat._24;
	float m31 = mat._31;
	float m32 = mat._32;
	float m33 = mat._33;
	float m34 = mat._34;
	float m41 = mat._41;
	float m42 = mat._42;
	float m43 = mat._43;
	float m44 = mat._44;
	float single = m33 * m44 - m34 * m43;
	float single1 = m32 * m44 - m34 * m42;
	float single2 = m32 * m43 - m33 * m42;
	float single3 = m31 * m44 - m34 * m41;
	float single4 = m31 * m43 - m33 * m41;
	float single5 = m31 * m42 - m32 * m41;
	float single6 = m22 * single - m23 * single1 + m24 * single2;
	float single7 = -(m21 * single - m23 * single3 + m24 * single4);
	float single8 = m21 * single1 - m22 * single3 + m24 * single5;
	float single9 = -(m21 * single2 - m22 * single4 + m23 * single5);
	float single10 = 1.0f / (m11 * single6 + m12 * single7 + m13 * single8 + m14 * single9);
	matrix1._11 = single6 * single10;
	matrix1._21 = single7 * single10;
	matrix1._31 = single8 * single10;
	matrix1._41 = single9 * single10;
	matrix1._12 = -(m12 * single - m13 * single1 + m14 * single2) * single10;
	matrix1._22 = (m11 * single - m13 * single3 + m14 * single4) * single10;
	matrix1._32 = -(m11 * single1 - m12 * single3 + m14 * single5) * single10;
	matrix1._42 = (m11 * single2 - m12 * single4 + m13 * single5) * single10;
	float single11 = m23 * m44 - m24 * m43;
	float single12 = m22 * m44 - m24 * m42;
	float single13 = m22 * m43 - m23 * m42;
	float single14 = m21 * m44 - m24 * m41;
	float single15 = m21 * m43 - m23 * m41;
	float single16 = m21 * m42 - m22 * m41;
	matrix1._13 = (m12 * single11 - m13 * single12 + m14 * single13) * single10;
	matrix1._23 = -(m11 * single11 - m13 * single14 + m14 * single15) * single10;
	matrix1._33 = (m11 * single12 - m12 * single14 + m14 * single16) * single10;
	matrix1._43 = -(m11 * single13 - m12 * single15 + m13 * single16) * single10;
	float single17 = m23 * m34 - m24 * m33;
	float single18 = m22 * m34 - m24 * m32;
	float single19 = m22 * m33 - m23 * m32;
	float single20 = m21 * m34 - m24 * m31;
	float single21 = m21 * m33 - m23 * m31;
	float single22 = m21 * m32 - m22 * m31;
	matrix1._14 = -(m12 * single17 - m13 * single18 + m14 * single19) * single10;
	matrix1._24 = (m11 * single17 - m13 * single20 + m14 * single21) * single10;
	matrix1._34 = -(m11 * single18 - m12 * single20 + m14 * single22) * single10;
	matrix1._44 = (m11 * single19 - m12 * single21 + m13 * single22) * single10;
	return matrix1;
}
