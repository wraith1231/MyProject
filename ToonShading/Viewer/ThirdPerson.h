#pragma once
#include "Camera.h"

class ThirdPerson : public Camera
{
public:
	ThirdPerson();
	~ThirdPerson();

	void Update();

private:

	float camDis;

	D3DXVECTOR2 camRot;
	float distance;
};