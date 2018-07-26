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

private:
	class GameSettings* settings;
};