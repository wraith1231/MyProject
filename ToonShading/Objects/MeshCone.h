#pragma once
#include "GameModel.h"

class MeshCone : public GameModel
{
public:
	MeshCone(ExecuteValues* values = NULL);
	~MeshCone();

	void Update();
	void PreRender();

	void ImGuiRender();

private:

};
