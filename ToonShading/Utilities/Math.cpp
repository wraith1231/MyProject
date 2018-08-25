#include "stdafx.h"
#include "Math.h"

const float Math::PI = 3.14159265f;

D3DXVECTOR3 Math::Max(D3DXVECTOR3 val1, D3DXVECTOR3 val2)
{
	D3DXVECTOR3 ret;

	ret.x = val1.x > val2.x ? val1.x : val2.x;
	ret.y = val1.y > val2.y ? val1.y : val2.y;
	ret.z = val1.z > val2.z ? val1.z : val2.z;

	return ret;
}

D3DXVECTOR3 Math::Min(D3DXVECTOR3 val1, D3DXVECTOR3 val2)
{
	D3DXVECTOR3 ret;

	ret.x = val1.x < val2.x ? val1.x : val2.x;
	ret.y = val1.y < val2.y ? val1.y : val2.y;
	ret.z = val1.z < val2.z ? val1.z : val2.z;

	return ret;
}

float Math::Modulo(float val1, float val2)
{
	while (val1 - val2 >= 0)
		val1 -= val2;

	return val1;
}

float Math::ToRadian(float degree)
{
	return degree * PI / 180.0f;
}

float Math::ToDegree(float radian)
{
	return radian * 180.0f / PI;
}

float Math::Random(float r1, float r2)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = r2 - r1;
	float val = random * diff;

	return r1 + val;
}

int Math::Random(int r1, int r2)
{
	return (int)(rand() % (r2 - r1 + 1)) + r1;
}

float Math::Clamp(float value, float min, float max)
{
	value = value > max ? max : value;
	value = value < min ? min : value;

	return value;
}

void Math::GetDistance(D3DXVECTOR3 v1, D3DXVECTOR3 v2, float & result)
{
	float x = v1.x - v2.x;
	float y = v1.y - v2.y;
	float z = v1.z - v2.z;
	float single = x * x + y * y + z * z;
	result = sqrt(single);
}

void Math::GetLerp(D3DXVECTOR3 v1, D3DXVECTOR3 v2, float amount, D3DXVECTOR3 & result)
{
	result.x = v1.x + (v2.x - v1.x) * amount;
	result.y = v1.y + (v2.y - v1.y) * amount;
	result.z = v1.z + (v2.z - v1.z) * amount;
}

D3DXVECTOR3 Math::Clamp(D3DXVECTOR3 val1, D3DXVECTOR3 min, D3DXVECTOR3 max)
{
	float x = val1.x;
	x = (x > max.x ? max.x : x);
	x = (x < min.x ? min.x : x);
	float y = val1.y;
	y = (y > max.y ? max.y : y);
	y = (y < min.y ? min.y : y);
	float z = val1.z;
	z = (z > max.z ? max.z : z);
	z = (z < min.z ? min.z : z);

	return D3DXVECTOR3(x, y, z);
}

float Math::DistanceSquared(D3DXVECTOR3 val1, D3DXVECTOR3 val2)
{
	float x = val1.x - val2.x;
	float y = val1.y - val2.y;
	float z = val1.z - val2.z;

	return x*x + y*y + z*z;
}

float Math::Distance(D3DXVECTOR3 v1, D3DXVECTOR3 v2)
{
	D3DXVECTOR3 v = v1 - v2;
	float result = v.x * v.x + v.y * v.y + v.z * v.z;

	return sqrtf(result);
}
D3DXMATRIX Math::CreateLookAt(D3DXVECTOR3 pos, D3DXVECTOR3 target, D3DXVECTOR3 up)
{
	D3DXMATRIX m;
	D3DXMatrixIdentity(&m);
	D3DXVECTOR3 vec3, vec31, vec32, vec31c;
	D3DXVec3Normalize(&vec3, &D3DXVECTOR3(pos - target));
	D3DXVec3Cross(&vec31c, &up, &vec3);
	D3DXVec3Normalize(&vec31, &vec31c);
	D3DXVec3Cross(&vec32, &vec3, &vec31);

	float dotx, doty, dotz;
	dotx = -D3DXVec3Dot(&vec31, &pos);
	doty = -D3DXVec3Dot(&vec32, &pos);
	dotz = -D3DXVec3Dot(&vec3, &pos);

	m = D3DXMATRIX
	{
		vec31.x, vec32.x, vec3.x, 0.0f,
		vec31.y, vec32.y, vec3.y, 0.0f,
		vec31.z, vec32.z, vec3.z, 0.0f,
		dotx, doty, dotz, 1.0f
	}; 

	return m;
}
D3DXMATRIX Math::CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
{
	D3DXMATRIX m;
	D3DXMatrixIdentity(&m);
	m._11 = 2.0f / (right - left);
	m._22 = 2.0f / (top - bottom);
	m._33 = 1.0f / (zNearPlane - zFarPlane);
	m._41 = (left + right) / (left - right);
	m._42 = (top + bottom) / (bottom - top);
	m._43 = zNearPlane / (zNearPlane - zFarPlane);
	m._44 = 1.0f;

	return m;
}
D3DXVECTOR3 Math::Transform(D3DXVECTOR3 pos, D3DXMATRIX mat)
{
	float x = pos.x * mat._11 + pos.y * mat._21 + pos.z * mat._31 + mat._41;
	float y = pos.x * mat._12 + pos.y * mat._22 + pos.z * mat._32 + mat._42;
	float z = pos.x * mat._13 + pos.y * mat._23 + pos.z * mat._33 + mat._43;

	return D3DXVECTOR3(x, y, z);
}
/*
float Math::DistanceSquared(D3DXVECTOR3 v1, D3DXVECTOR3 v2)
{
	D3DXVECTOR3 v = v1 - v2;
	float result = v.x * v.x + v.y * v.y + v.z * v.z;

	return result;
	//캐릭터간 판단할 거리를 제곱으로 해서 계산하는게 훨씬 빠름
}
*/