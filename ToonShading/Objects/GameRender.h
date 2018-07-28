#pragma once

class GameRender
{
public:
	GameRender();
	virtual ~GameRender();

	void Enable(bool val);
	bool Enable();

	void Visible(bool val);
	bool Visible();

	vector<function<void()>> Enabled;	//unity���� �����״� �ϴ� �� ���
										//�������� ������ ���� ��(update)
	vector<function<void()>> Visibled;	//visibled�� render���� ����

	void RootAxis(D3DXMATRIX& mat);
	D3DXMATRIX RootAxis();

	void World(D3DXMATRIX& mat);
	D3DXMATRIX World();

	void Position(D3DXVECTOR3& vec);
	D3DXVECTOR3 Position();

	void Direction(D3DXVECTOR3& vec);
	D3DXVECTOR3 Direction();

	void Up(D3DXVECTOR3& vec);
	D3DXVECTOR3 Up();

	void Right(D3DXVECTOR3& vec);
	D3DXVECTOR3 Right();

	void Scale(float& val);
	float Scale();

	D3DXMATRIX Transformed();

	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void PreRender() = 0;
	virtual	void PreRender2() = 0;
	virtual void ImGuiRender() = 0;

	D3DXVECTOR3* FollowPos();

protected:
	bool enable;
	bool visible;

	D3DXMATRIX rootAxis;

private:
	D3DXMATRIX world;

	D3DXVECTOR3 scale;

	D3DXVECTOR3 position;
	D3DXVECTOR3 direction;
	D3DXVECTOR3 up;
	D3DXVECTOR3 right;
	//dir, up, right�� quaternion���� �� ���� ������ �Ϻη� ���ͷ� ��
};