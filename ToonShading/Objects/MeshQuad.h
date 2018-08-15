#pragma once
#include "GameModel.h"

class MeshQuad : public GameModel
{
public:
	MeshQuad(ExecuteValues* values = NULL);
	~MeshQuad();

	void Update();
	void PreRender();

	void ImGuiRender();

private:

};
