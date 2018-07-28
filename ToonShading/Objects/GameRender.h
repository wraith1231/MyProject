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

	vector<function<void()>> Enabled;	//unity에서 껐다켰다 하는 그 기능
										//수행할지 말지에 대한 것(update)
	vector<function<void()>> Visibled;	//visibled는 render할지 말지

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
	//dir, up, right는 quaternion으로 쓸 수도 있지만 일부러 벡터로 함
};