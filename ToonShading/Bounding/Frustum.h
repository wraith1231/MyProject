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

		void ConstructFrustum(float screenFar, D3DXMATRIX projection, D3DXMATRIX view);

		bool CheckBox(Objects::BoundingBox* box);
		bool CheckPoint(float, float, float);
		bool CheckCube(float, float, float, float);
		bool CheckSphere(float, float, float, float);
		bool CheckRectangle(float, float, float, float, float, float);

	private:
		D3DXPLANE planes[6];

	};
}