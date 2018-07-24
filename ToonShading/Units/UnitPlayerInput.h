#pragma once
#include "stdafx.h"

enum class GamePlayerKey
{
	MoveForward = 0, MoveBackward, MoveLeft, MoveRight,
	TurnLeft, TurnRight,
	WeaponFire, WeaponReload, WeaponChange,
	Boost,
	Count,
};

struct GamePlayerInput
{
private:
	//private이라 소문자로 씀
	UINT keyboard[(UINT)GamePlayerKey::Count];

public:
	GamePlayerInput()
	{
		keyboard[(UINT)GamePlayerKey::MoveForward] = 'W';
		keyboard[(UINT)GamePlayerKey::MoveBackward] = 'S';
		keyboard[(UINT)GamePlayerKey::MoveLeft] = 'A';
		keyboard[(UINT)GamePlayerKey::MoveRight] = 'D';

		keyboard[(UINT)GamePlayerKey::TurnLeft] = 'Q';
		keyboard[(UINT)GamePlayerKey::TurnRight] = 'E';

		keyboard[(UINT)GamePlayerKey::WeaponFire] = 'F';
		keyboard[(UINT)GamePlayerKey::WeaponReload] = 'R';
		keyboard[(UINT)GamePlayerKey::WeaponChange] = VK_TAB;

		keyboard[(UINT)GamePlayerKey::Boost] = VK_SPACE;
	}

	void SetGameKey(GamePlayerKey key, UINT setKey)
	{
		keyboard[(UINT)key] = setKey;
	}

	bool Pressed(GamePlayerKey key)
	{
		bool b = false;
		b |= Mouse::Get()->Press(1) == false;
		b &= Keyboard::Get()->Press(keyboard[(UINT)key]);

		return b;
		//return Keyboard::Get()->Press(keyboard[(UINT)key]);
	}

	bool Released(GamePlayerKey key)
	{
		bool b = false;
		b |= Mouse::Get()->Press(1) == false;
		b &= Keyboard::Get()->Up(keyboard[(UINT)key]);

		return b;
		//return Keyboard::Get()->Up(keyboard[(UINT)key]);
	}

	bool Stroke(GamePlayerKey key)
	{
		bool b = false;
		b |= Mouse::Get()->Press(1) == false;
		b &= Keyboard::Get()->Down(keyboard[(UINT)key]);

		return b;
		//return Keyboard::Get()->Down(keyboard[(UINT)key]);
	}

	bool IsPressTurn()
	{
		bool b = false;
		b |= Pressed(GamePlayerKey::TurnLeft);
		b |= Pressed(GamePlayerKey::TurnRight);

		return b;
		//return Pressed(GamePlayerKey::TurnLeft) || Pressed(GamePlayerKey::TurnRight);
	}

	bool IsPressMovement()
	{
		bool b = false;
		b |= Pressed(GamePlayerKey::MoveForward);
		b |= Pressed(GamePlayerKey::MoveBackward);
		b |= Pressed(GamePlayerKey::MoveLeft);
		b |= Pressed(GamePlayerKey::MoveRight);

		return b;
	}
};