#pragma once
#include "Camera.h"

class FreeCam : public Camera
{
public:
	FreeCam(float moveSpeed = 20, float rotationSpeed = 2.5f);
	~FreeCam();

	void Update();

private:
	float moveSpeed;
	float rotationSpeed;
};