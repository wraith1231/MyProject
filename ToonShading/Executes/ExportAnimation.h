#pragma once

class ExportAnimation : public Execute
{
public:
	ExportAnimation(ExecuteValues* values);
	~ExportAnimation();

	void Update();

	void PreRender();
	void Render();
	void PostRender();
	void PreRender2() {}
	void ImGuiRender();

	void ResizeScreen() {}

private:

};