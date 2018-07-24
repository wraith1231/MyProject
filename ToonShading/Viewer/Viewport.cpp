#include "stdafx.h"
#include "Viewport.h"

Viewport::Viewport(float width, float height, float x, float y, float minDepth, float maxDepth)
{
	Set(width, height, x, y, minDepth, maxDepth);
}

Viewport::~Viewport()
{
	
}

void Viewport::Set(float width, float height, float x, float y, float minDepth, float maxDepth)
{
	viewport.TopLeftX = this->x = x;
	viewport.TopLeftY = this->y = y;
	viewport.Width = this->width = width;
	viewport.Height = this->height = height;
	viewport.MinDepth = this->maxDepth = minDepth;
	viewport.MaxDepth = this->maxDepth = maxDepth;

	RSSetViewport();
}

D3DXVECTOR3 Viewport::Unproject(D3DXVECTOR3 src, D3DXMATRIX projection, D3DXMATRIX view, D3DXMATRIX world)
{
	D3DXMATRIX mat;
	D3DXMatrixMultiply(&mat, &world, &view);
	D3DXMatrixMultiply(&mat, &mat, &projection);

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

	src.x = (src.x - x) / width*2.0f - 1.0f;
	src.y = -((src.y - y) / height * 2.0f - 1.0f);
	src.z = (src.z - minDepth) / (maxDepth - minDepth);

	D3DXVECTOR3 vec;
	D3DXVec3TransformCoord(&vec, &src, &mat);
	float x = src.x * mat._14 + src.y * mat._24 + src.z * mat._34 + mat._44;

	if (fabs(x - 1.0f) < 0.0001f)
		vec /= x;

	return vec;
}

void Viewport::RSSetViewport()
{
	D3D::GetDC()->RSSetViewports(1, &viewport);
}
