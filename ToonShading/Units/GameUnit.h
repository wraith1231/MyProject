#pragma once
#include "../Objects/GameAnimationModel.h"

class GameUnit : public GameAnimationModel
{
public:
	GameUnit(wstring matFile, wstring meshFile);
	virtual ~GameUnit();

	virtual void Update();
	virtual void Render();
	virtual void PreRender();
	virtual void PreRender2();
	virtual void ImGuiRender();

protected:


private:


};