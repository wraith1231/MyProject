#pragma once

class ModelBone;

class DrawModel : public Execute
{
public:
	DrawModel(ExecuteValues* values);
	~DrawModel();

	void Update();

	void PreRender();
	void PreRender2();
	void Render();
	void PostRender();
	void PostRender2();
	void ImGuiRender();

	void ResizeScreen() {}

	D3DXVECTOR3* GetCharPos();

private:
	void CreateEnvironment();
	void CreateAnimationModel();
	void CreatePlayer();
	void CreateEnemy();

private:
	class GameAnimationModel* model;
	class Model* pModel;
	class ModelAnimPlayer* aPlayer;

	class GameAnimationModel* grund;
	class GamePlayer* player;
	class GameEnemy* enemy;
	class GameWorld* world;
	class GameSkyBox* skybox;
};