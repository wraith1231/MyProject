#pragma once

//얘는 직교투영
//perspective는 원교투영

class Orthographic
{
public:	
	//얘는 left right minz maxz near far만 들어감
	Orthographic(float left, float right,
		float bottom, float top,
		float zn = 0.0f, float zf = 1.0f);
	~Orthographic();

	void GetMatrix(D3DXMATRIX* mat);
	void Set(float left, float right,
		float bottom, float top,
		float zn = 0.0f, float zf = 1.0f);

private:
	float left, right;
	float bottom, top;
	float zn, zf;

	D3DXMATRIX matrix;
};