#pragma once

//계산만 할 것
namespace Objects
{
	class Frustum
	{
	public:
		Frustum();
		Frustum(const Frustum&);
		~Frustum();

		void ConstructFrustum(float, D3DXMATRIX, D3DXMATRIX);

		bool CheckPoint(float, float, float);
		bool CheckCube(float, float, float, float);
		bool CheckSphere(float, float, float, float);
		bool CheckRectangle(float, float, float, float, float, float);

	private:
		D3DXPLANE planes[6];

	};
}