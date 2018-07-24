#include "stdafx.h"
#include "FreeCam.h"

FreeCam::FreeCam(float moveSpeed, float rotationSpeed)
	: moveSpeed(moveSpeed), rotationSpeed(rotationSpeed)
{

}

FreeCam::~FreeCam()
{

}

void FreeCam::Update()
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
			position += forward * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			position += -forward * moveSpeed * Time::Delta();

		if (Keyboard::Get()->Press('A'))
			position += -right * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('D'))
			position += right * moveSpeed * Time::Delta();

		if (Keyboard::Get()->Press('E'))
			position += up * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('Q'))
			position += -up * moveSpeed * Time::Delta();

		SetPosition(position.x, position.y, position.z);
	}

	//Rotation
	{
		D3DXVECTOR2 rotation;
		GetRotation(&rotation);

		if (Mouse::Get()->Press(1))
		{
			D3DXVECTOR3 move = Mouse::Get()->GetMoveValue();

			rotation.x += move.y * rotationSpeed * Time::Delta();
			rotation.y += move.x * rotationSpeed * Time::Delta();
		}

		SetRotation(rotation.x, rotation.y);
	}
}
