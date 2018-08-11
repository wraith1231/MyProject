#include "stdafx.h"
#include "Frustum.h"

#include "BoundingBox.h"

Objects::Frustum::Frustum()
{
}

Objects::Frustum::Frustum(const Frustum &)
{
}

Objects::Frustum::~Frustum()
{
}

void Objects::Frustum::ConstructFrustum(float screenFar, D3DXMATRIX projection, D3DXMATRIX view)
{
	float zMinimum, r;
	D3DXMATRIX mat;

	zMinimum = -projection._43 / projection._33;
	r = screenFar / (screenFar - zMinimum);

	projection._33 = r;
	projection._43 = -r * zMinimum;

	D3DXMatrixMultiply(&mat, &view, &projection);

	planes[0].a = mat._14 + mat._13;
	planes[0].b = mat._24 + mat._23;
	planes[0].c = mat._34 + mat._33;
	planes[0].d = mat._44 + mat._43;
	D3DXPlaneNormalize(&planes[0], &planes[0]);

	planes[1].a = mat._14 - mat._13;
	planes[1].b = mat._24 - mat._23;
	planes[1].c = mat._34 - mat._33;
	planes[1].d = mat._44 - mat._43;
	D3DXPlaneNormalize(&planes[1], &planes[1]);

	planes[2].a = mat._14 + mat._11;
	planes[2].b = mat._24 + mat._21;
	planes[2].c = mat._34 + mat._31;
	planes[2].d = mat._44 + mat._41;
	D3DXPlaneNormalize(&planes[2], &planes[2]);

	planes[3].a = mat._14 - mat._11;
	planes[3].b = mat._24 - mat._21;
	planes[3].c = mat._34 - mat._31;
	planes[3].d = mat._44 - mat._41;
	D3DXPlaneNormalize(&planes[3], &planes[3]);

	planes[4].a = mat._14 - mat._12;
	planes[4].b = mat._24 - mat._22;
	planes[4].c = mat._34 - mat._32;
	planes[4].d = mat._44 - mat._42;
	D3DXPlaneNormalize(&planes[4], &planes[4]);

	planes[5].a = mat._14 + mat._12;
	planes[5].b = mat._24 + mat._22;
	planes[5].c = mat._34 + mat._32;
	planes[5].d = mat._44 + mat._42;
	D3DXPlaneNormalize(&planes[5], &planes[5]);
}

bool Objects::Frustum::CheckBox(Objects::BoundingBox * box)
{
	bool flag = false;

	for (int i = 0; i < 6; i++)
	{
		bool c = box->Intersects(planes[i]);

		if (c == false)
		{
			return false;
		}

		if (c == true)
		{
			flag = true;
		}
	}

	if (flag == false)
		return false;

	return true;
}

bool Objects::Frustum::CheckPoint(float x, float y, float z)
{
	for (int i = 0; i < 6; i++)
	{
		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3(x, y, z)) < 0.0f)
			return false;
	}

	return true;
}

bool Objects::Frustum::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	for (int i = 0; i < 6; i++)
	{
		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter - radius), (zCenter - radius))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter - radius), (zCenter - radius))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter + radius), (zCenter - radius))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter + radius), (zCenter - radius))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter - radius), (zCenter + radius))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter - radius), (zCenter + radius))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter + radius), (zCenter + radius))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter + radius), (zCenter + radius))) >= 0.0f)
			continue;

		return false;
	}

	return true;
}

bool Objects::Frustum::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	for (int i = 0; i < 6; i++)
	{
		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3(xCenter, yCenter, zCenter)) < -radius)
			return false;
	}

	return true;
}

bool Objects::Frustum::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	for (int i = 0; i < 6; i++)
	{
		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter - ySize), (zCenter - zSize))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter - ySize), (zCenter - zSize))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter + ySize), (zCenter - zSize))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter + ySize), (zCenter - zSize))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter - ySize), (zCenter + zSize))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter - ySize), (zCenter + zSize))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter + ySize), (zCenter + zSize))) >= 0.0f)
			continue;

		if (D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter + ySize), (zCenter + zSize))) >= 0.0f)
			continue;

		return false;
	}

	return true;
}
