#pragma once
class Math
{
public:
	static const float PI;

	static float Modulo(float val1, float val2);

	static float ToRadian(float degree);
	static float ToDegree(float radian);

	static int Random(int r1, int r2);
	static float Random(float r1, float r2);

	static float Clamp(float value, float min, float max);

	static void GetDistance(D3DXVECTOR3 v1, D3DXVECTOR3 v2, float& result);
	static void GetLerp(D3DXVECTOR3 v1, D3DXVECTOR3 v2, float amount, D3DXVECTOR3& result);
	static D3DXVECTOR3 Clamp(D3DXVECTOR3 val1, D3DXVECTOR3 min, D3DXVECTOR3 max);
	static float DistanceSquared(D3DXVECTOR3 val1, D3DXVECTOR3 val2);
};