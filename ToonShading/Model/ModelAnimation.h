#pragma once

class ModelAnimation
{
public:
	friend class Model;

public:
	ModelAnimation(bool loop, wstring name, vector<float> times, vector<D3DXVECTOR3> scale, vector<D3DXVECTOR3> rotate, vector<D3DXVECTOR3> trans);
	~ModelAnimation();

	void Update();

	wstring GetName() { return name; }

	D3DXMATRIX GetTransform();

	//float GetDelta() { return deltaa; }
	//float GetFrameDelta() { return frameTime; }
	//float GetTDelta() { return tDeltaa; }

	//D3DXQUATERNION GetQua() { return q; }

	//D3DXVECTOR3 GetS() { return s; }
	//D3DXVECTOR3 GetR() { return r; }
	//D3DXVECTOR3 GetT() { return t; }

	//D3DXMATRIX GetRMat() { return R; }

private:
	vector<D3DXVECTOR3> scale, rotate, trans;

	//D3DXVECTOR3 s, r, t;

	bool loop, play;
	wstring name;
	vector<float> time;

	UINT index;
	//float deltaa;
	float frameTime;

	//float tDeltaa;

	//D3DXQUATERNION q;

	D3DXMATRIX transform;

	//D3DXMATRIX R;
};