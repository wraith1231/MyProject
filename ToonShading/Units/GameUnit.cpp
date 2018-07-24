#include "stdafx.h"
#include "GameUnit.h"

GameUnit::GameUnit(wstring matFile, wstring meshFile)
	: GameAnimationModel(matFile, meshFile)
{

}

GameUnit::~GameUnit()
{

}

void GameUnit::Update()
{
	GameAnimationModel::Update();
}

void GameUnit::Render()
{
	GameAnimationModel::Render();
}

void GameUnit::PreRender()
{
	GameAnimationModel::PreRender();
}

void GameUnit::PreRender2()
{
	GameAnimationModel::PreRender2();
}

void GameUnit::ImGuiRender()
{
}
