#pragma once

class Program
{
public:
	Program();
	~Program();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen();

private:
	void SetGlobalBuffers();

private:
	ExecuteValues* values;
	vector<class Execute *> executes;

	Objects::BoundingBox* box;
	Objects::Ray* ray;
	D3DXVECTOR3 light, cen;
};