#pragma once
#include "GameModel.h"

class GameSkyBox : public GameModel
{
public:
	GameSkyBox(wstring matFile, wstring meshFile);
	~GameSkyBox();

	void SetBasisPosition(D3DXVECTOR3 vec);

	void FollowCamera(bool val);

private:
	bool bFollowCamera;	//skybox 만들때 카메라 화면 기준으로 따라갔는데 이젠 그냥 고정으로 할 것
						//따라가는건 알아서 만들기

};