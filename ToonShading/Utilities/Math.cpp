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
/*
float Math::DistanceSquared(D3DXVECTOR3 v1, D3DXVECTOR3 v2)
{
	D3DXVECTOR3 v = v1 - v2;
	float result = v.x * v.x + v.y * v.y + v.z * v.z;

	return result;
	//캐릭터간 판단할 거리를 제곱으로 해서 계산하는게 훨씬 빠름
}
*/