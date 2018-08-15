#pragma once
#include "GameModel.h"

class MeshPlane : public GameModel
{
public:
	MeshPlane(ExecuteValues* values = NULL);
	~MeshPlane();

	void Update();
	void PreRender();

	void ImGuiRender();

private:

};
