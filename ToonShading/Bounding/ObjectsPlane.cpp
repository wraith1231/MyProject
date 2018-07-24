#include "stdafx.h"
#include "ObjectsPlane.h"
#include "ObjectsRay.h"

Objects::Plane::Plane()
{
	Normal.x = 0;
	Normal.y = 0;
	Normal.z = 0;
	D = 0;
}

Objects::Plane::Plane(float a, float b, float c, float d)
{
	Normal.x = a;
	Normal.y = b;
	Normal.z = c;
	D = d;
}

Objects::Plane::Plane(D3DXVECTOR3 vec, float d)
{
	Normal = vec;
	D = d;
}

Objects::Plane::Plane(D3DXVECTOR3 v1, D3DXVECTOR3 v2, D3DXVECTOR3 v3)
{
	float x = v2.x - v1.x;
	float y = v2.y - v1.y;
	float z = v2.z - v1.z;
	float single = v3.x - v1.x;
	float y1 = v3.y - v1.y;
	float z1 = v3.z - v1.z;
	float single1 = y*z1 - z*y1;
	float single2 = z*single - x*z1;
	float single3 = x*y1 - y*single;
	float single4 = single1 * single1 + single2 * single2 + single3 * single3;
	float single5 = 1.f / (float)sqrt(single4);

	Normal.x = single1 * single5;
	Normal.y = single2 * single5;
	Normal.z = single3 * single5;
	D = -(Normal.x * v1.x + Normal.y * v1.y + Normal.z * v1.z);
}

Objects::Plane::~Plane()
{

}

float Objects::Plane::Dot(Plane src)
{
	return Normal.x * src.Normal.x + Normal.y * src.Normal.y + Normal.z * src.Normal.z + D;
}

float Objects::Plane::DotNormal(Plane src)
{
	return Normal.x * src.Normal.x + Normal.y * src.Normal.y + Normal.z * src.Normal.z;
}

Objects::Plane* Objects::Plane::Normalize()
{
	Plane* normal = new Plane();
	float x = Normal.x * Normal.x + Normal.y * Normal.y + Normal.z * Normal.z;

	if (abs(x - 1.f) < 0.0001f)
	{
		normal->Normal = Normal;
		normal->D = D;
		return normal;
	}

	float single = 1.f / (float)sqrt(x);
	normal->Normal.x = Normal.x * single;
	normal->Normal.y = Normal.y * single;
	normal->Normal.z = Normal.z * single;
	normal->D = D * single;
	return normal;
}
