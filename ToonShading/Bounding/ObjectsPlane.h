#pragma once

namespace Objects
{
	class Plane
	{
	public:
		Plane();
		Plane(float a, float b, float c, float d);
		Plane(D3DXVECTOR3 vec, float d);
		Plane(D3DXVECTOR3 v1, D3DXVECTOR3 v2, D3DXVECTOR3 v3);

		~Plane();

		float Dot(Plane src);
		float DotNormal(Plane src);
		Objects::Plane* Normalize();

	public:
		D3DXVECTOR3 Normal;
		float D;
	};

}