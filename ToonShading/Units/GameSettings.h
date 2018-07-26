#pragma once
#include "GamePlayer.h"

class GameSettings
{
public:
	GameSettings();
	~GameSettings();

	void Update();
	void Render();

	void PreRender();
	void PreRender2();
	void PostRender();
	void ImguiRender();

	static class GamePlayer* Player() { return player; }

private:
	void CreateEnvironment();
	void CreatePlayer();
	void CreateEnemy();

private:
	class GameWorld* world;
	class GameSkyBox* skyBox;

	static class GamePlayer* player;
	class GameEnemy* enemy;
	
};