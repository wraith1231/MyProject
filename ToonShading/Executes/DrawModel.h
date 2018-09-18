#pragma once

class ModelBone;

class DrawModel : public Execute
{
public:
	DrawModel(ExecuteValues* values);
	~DrawModel();

	void Update();

	void ShadowRender();
	void PreRender();
	void LightRender();
	void SSAORender();
	void EdgeRender();
	void AARender();
	void ImGuiRender();

	void ResizeScreen() {}

private:
	class GameSettings* settings;

	//class ModelAnimPlayer* player;
	//class Model* model;
};