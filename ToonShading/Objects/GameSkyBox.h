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
	bool bFollowCamera;	//skybox ���鶧 ī�޶� ȭ�� �������� ���󰬴µ� ���� �׳� �������� �� ��
						//���󰡴°� �˾Ƽ� �����

};