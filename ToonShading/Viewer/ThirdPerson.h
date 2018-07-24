#pragma once
#include "Camera.h"

class ThirdPerson : public Camera
{
public:
	ThirdPerson(D3DXVECTOR3* target);
	~ThirdPerson();

	void Update();

private:
	D3DXVECTOR3* target;

	float camDis;

	D3DXVECTOR2 camRot;
	float distance;
};