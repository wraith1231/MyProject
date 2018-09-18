#pragma once

class ExportMesh : public Execute
{
public:
	ExportMesh(ExecuteValues* values);
	~ExportMesh();

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

};