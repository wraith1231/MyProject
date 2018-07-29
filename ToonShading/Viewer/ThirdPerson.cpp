#include "stdafx.h"
#include "ThirdPerson.h"

ThirdPerson::ThirdPerson()
{
	camRot = D3DXVECTOR2(0, 0);
	camDis = 10.0f;
	distance = 10.0f;
}

ThirdPerson::~ThirdPerson()
{
}

void ThirdPerson::Update()
{
	if (target != NULL)
	{

		float delta = Time::Delta();
		if (Mouse::Get()->Press(1) == true)
		{
			if (Keyboard::Get()->Press('Q'))
			{
				camDis -= 5.0f * delta;
			}
			if (Keyboard::Get()->Press('E'))
			{
				camDis += 5.0f * delta;
			}
		}

		D3DXVECTOR3 wheel = Mouse::Get()->WheelMoveValue();

		if (wheel.z > 0) camDis -= 30.0f * delta;
		else if (wheel.z < 0) camDis += 30.0f * delta;

		if (camDis <= 5.0f) camDis = 5.0f;
		if (camDis >= 12.0f) camDis = 12.0f;

		if (distance < camDis) distance += 5.0f * delta;
		if (distance > camDis) distance -= 5.0f * delta;

		if (Mouse::Get()->Press(1))
		{
			D3DXVECTOR3 move = Mouse::Get()->GetMoveValue();

			camRot.x += move.y * 1.0f * delta;
			camRot.y += move.x * 1.0f * delta;

			if (camRot.x < -(float)D3DX_PI / 9) camRot.x = (float)-D3DX_PI / 9;
			if (camRot.x >= (float)D3DX_PI / 9) camRot.x = (float)D3DX_PI / 9;
		}
		D3DXMATRIXA16 mxR, mxRX, mxRY;
		D3DXMatrixRotationX(&mxRX, camRot.x);
		D3DXMatrixRotationY(&mxRY, camRot.y);

		mxR = mxRX * mxRY;

		D3DXVECTOR3 eye, up;
		up = D3DXVECTOR3(0, 1, 0);
		eye = D3DXVECTOR3(0, 5.0f, -distance);
		D3DXVec3TransformCoord(&eye, &eye, &mxR);

		D3DXVECTOR3 lookAt;
		if (target != NULL)
		{
			lookAt = *target;
			eye = eye + *target;
		}

		SetPosition(eye.x, eye.y, eye.z);
		SetRotation(camRot.x, camRot.y);		//지금 캐릭터가 화면이랑 같은 방향을 보게 하기 위해서 저장해둠

		D3DXMATRIXA16 matView;
		D3DXMatrixLookAtLH(&matView, &eye, &lookAt, &up);

		SetView(matView);
	}
	else
	{
		D3DXVECTOR3 forward, right, up;
		GetForward(&forward);
		GetRight(&right);
		GetUp(&up);

		//Translation
		{
			D3DXVECTOR3 position;
			GetPosition(&position);

			if (Mouse::Get()->Press(1) == false)
				return;


			if (Keyboard::Get()->Press('W'))
				position += forward * 100.0f * Time::Delta();
			else if (Keyboard::Get()->Press('S'))
				position += -forward * 100.0f * Time::Delta();

			if (Keyboard::Get()->Press('A'))
				position += -right * 100.0f * Time::Delta();
			else if (Keyboard::Get()->Press('D'))
				position += right * 100.0f * Time::Delta();

			if (Keyboard::Get()->Press('E'))
				position += up * 100.0f * Time::Delta();
			else if (Keyboard::Get()->Press('Q'))
				position += -up * 100.0f * Time::Delta();

			SetPosition(position.x, position.y, position.z);
		}

		//Rotation
		{
			D3DXVECTOR2 rotation;
			GetRotation(&rotation);

			if (Mouse::Get()->Press(1))
			{
				D3DXVECTOR3 move = Mouse::Get()->GetMoveValue();

				rotation.x += move.y * 2.5f * Time::Delta();
				rotation.y += move.x * 2.5f * Time::Delta();
			}

			SetRotation(rotation.x, rotation.y);
		}
	}
}
