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
	void SetLightBuffer();

private:
	ExecuteValues* values;
	vector<class Execute *> executes;
};