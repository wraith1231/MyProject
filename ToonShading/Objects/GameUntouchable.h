#pragma once

namespace Objects
{
	class BoundingBox;
	class Ray;
}

struct ModelStruct
{
	D3DXVECTOR3 Scale;
	D3DXVECTOR3 Rotate;
	D3DXVECTOR3 Translation;
	D3DXMATRIX Transforms;
};

class GameUntouchable
{
public:
	GameUntouchable(wstring file);
	~GameUntouchable();

	void AddTransforms(D3DXVECTOR3 scale, D3DXVECTOR3 rotate, D3DXVECTOR3 trans);

	bool SelectObject(Objects::Ray* ray);
	bool ObjectCheck(Objects::Ray* ray);

	void Update();
	void SelectObjectUpdate();
	void Unselect();

	void PreRender();
	void PreRender2();
	void Render();

	void ImGuiRender();

	void SaveTransforms(Json::Value* val, UINT num);

	UINT Transforms();

	Objects::BoundingBox* Box(UINT num);

	wstring GetFileName() { return file; }

private:
	bool selected;
	UINT selectNum;
	UINT setMode;

	Objects::BoundingBox* box;

	class Model* model;
	wstring file;

	Shader* shader;
	Shader* shader2;
	Shader* shader3;

	vector<ModelStruct*> transforms;
};