#pragma once
#include "GameModel.h"

class MeshCylinder : public GameModel
{
public:
	MeshCylinder(ExecuteValues* values = NULL);
	~MeshCylinder();

	void Update();
	void PreRender();

	void ImGuiRender();

private:

};
