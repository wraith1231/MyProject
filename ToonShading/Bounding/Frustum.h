#pragma once

//계산만 할 것
namespace Objects
{
	class Ray;
	class BoundingBox;

	class Frustum
	{
	public:
		Frustum();
		Frustum(const Frustum&);
		~Frustum();

		void ConstructFrustum(float screenFar, D3DXMATRIX projection, D3DXMATRIX view);

		D3DXVECTOR3 ComputeIntersection(D3DXPLANE plane, Objects::Ray* ray); 
		Objects::Ray* ComputeIntersection(D3DXPLANE p1, D3DXPLANE p2);
	
		bool CheckBox(Objects::BoundingBox* box);
		bool CheckPoint(float, float, float);
		bool CheckCube(float, float, float, float);
		bool CheckSphere(float, float, float, float);
		bool CheckRectangle(float, float, float, float, float, float);

		D3DXVECTOR3 GetCorner(UINT num);

	private:
		D3DXPLANE planes[6];

		D3DXVECTOR3 corner[8];
	};
}