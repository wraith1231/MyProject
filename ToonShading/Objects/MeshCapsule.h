#pragma once
#include "GameModel.h"

class MeshCapsule : public GameModel
{
public:
	MeshCapsule(ExecuteValues* values = NULL);
	~MeshCapsule();

	void Update();
	void PreRender();

	void ImGuiRender();

private:

};
