#pragma once
#include "GameModel.h"

class MeshCube : public GameModel
{
public:
	MeshCube(ExecuteValues* values = NULL);
	~MeshCube();

	void Update();
	void PreRender();

	void ImGuiRender();

private:

};
