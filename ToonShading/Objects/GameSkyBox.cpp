#include "stdafx.h"
#include "GameSkyBox.h"

GameSkyBox::GameSkyBox(wstring matFile, wstring meshFile)
	: GameModel(matFile, meshFile)
{

}

GameSkyBox::~GameSkyBox()
{
}

void GameSkyBox::SetBasisPosition(D3DXVECTOR3 vec)
{
	Position(vec);
}

void GameSkyBox::FollowCamera(bool val)
{
	bFollowCamera = val;
}
