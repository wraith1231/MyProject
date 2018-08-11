#pragma once
#include "GamePlayer.h"

struct ExecuteValues;
class GameSettings
{
public:
	GameSettings(ExecuteValues* values = NULL);
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
	static vector<class GameUntouchable*> Untouchables() { return untouchables; }

	void SetValues(ExecuteValues* value) { values = value; }

	void SaveScene();
	void LoadScene();

	void SceneSave(wstring file = L"");
	void SceneLoad(wstring file = L"");

private:
	void CreateEnvironment();
	void CreatePlayer();
	void CreateEnemy();
	void DisposeEnemy(D3DXVECTOR3 position);

	void DeleteAll();

private:
	ExecuteValues* values;

	class GameWorld* world;
	class GameSkyBox* skyBox;
	static class GameTerrain* terrain;

	static class GamePlayer* player;
	class GameEnemy* enemy;
	vector<class GameEnemy*> enemies;

	class GameAnimationModel* model;
	vector<class GameAnimationModel*> models;

	static vector<class GameUntouchable*> untouchables;

	bool inter, disposed, untouchSel;
	bool enemyDispose, playerDispose, enemySel;
	bool editMode;

	class GameUntouchable* untoTemp;

	wstring sceneFile;

	class Bullet* playerBullet;
	vector<class Bullet*> enemyBullets;
};