#pragma once
#include "GameModel.h"

class MeshSphere : public GameModel
{
public:
	MeshSphere(ExecuteValues* values = NULL);
	~MeshSphere();

	void Update();
	void PreRender();

	void ImGuiRender();
	void SetShader(wstring shaderName);

private:

};
