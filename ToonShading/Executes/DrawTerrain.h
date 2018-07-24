#pragma once

class Terrain;

class DrawTerrain : public Execute
{
public:
	DrawTerrain(ExecuteValues* values);
	~DrawTerrain();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen() {}

	Terrain* GetTerrain() { return terrain; }

private:
	Terrain* terrain;

};