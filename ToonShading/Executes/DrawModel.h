#pragma once

class ModelBone;

class DrawModel : public Execute
{
public:
	DrawModel(ExecuteValues* values);
	~DrawModel();

	void Update();

	void NormalRender();
	void DepthRender();
	void DiffuseRender();

	void LightRender();
	void EdgeRender();
	void AARender();
	void ImGuiRender();

	void ResizeScreen() {}

private:
	class GameSettings* settings;
};