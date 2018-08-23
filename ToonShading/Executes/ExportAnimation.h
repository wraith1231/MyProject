#pragma once

class ExportAnimation : public Execute
{
public:
	ExportAnimation(ExecuteValues* values);
	~ExportAnimation();

	void Update();

	void ShadowRender();
	void PreRender();
	void LightRender();
	void EdgeRender();
	void AARender();
	void ImGuiRender();

	void ResizeScreen() {}

private:

};