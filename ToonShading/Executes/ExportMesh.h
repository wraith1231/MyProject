#pragma once

class ExportMesh : public Execute
{
public:
	ExportMesh(ExecuteValues* values);
	~ExportMesh();

	void Update();

	void PreRender();
	void Render();
	void PostRender();
	void PreRender2() {}
	void PostRender2() {}
	void ImGuiRender();

	void ResizeScreen() {}

private:

};