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

	void DisposeUnTouchable(wstring file = L"");

	static class GamePlayer* Player() { return player; }
	static class GameTerrain* Terrain() { return terrain; }

	void SetValues(ExecuteValues* value) { values = value; }
private:
	void CreateEnvironment();
	void CreatePlayer();
	void CreateEnemy();

private:
	ExecuteValues* values;

	class GameWorld* world;
	class GameSkyBox* skyBox;
	static class GameTerrain* terrain;

	static class GamePlayer* player;
	class GameEnemy* enemy;

	class GameAnimationModel* model;
	vector<class GameAnimationModel*> models;

	vector<class GameUntouchable*> untouchables;

	bool inter, disposed, untouchSel;
	class GameUntouchable* untoTemp;
};