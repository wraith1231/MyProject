#pragma once

namespace Objects
{
	class BoundingSphere;
	class BoundingBox;
}

struct BulletStruct
{
	D3DXVECTOR3 Start;
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Direction;

	bool UseGravity;
	D3DXVECTOR3 GravityDir;
	D3DXVECTOR3 Gravity;
	float GravityPower;

	float Range;

	float Speed;
	D3DXMATRIX World;
};

class Bullet
{
public:
	Bullet();
	~Bullet();

	void AddBullet(D3DXVECTOR3 pos, D3DXVECTOR3 dir, float speed = 3.0f, float range = 50.0f, bool useGravity = false, D3DXVECTOR3 gravity = D3DXVECTOR3(0, 0, 0), float gravityPower = 9.8f);
	
	void Update();
	void Render();

	bool CheckIntersect(Objects::BoundingBox* box);

private:
	Objects::BoundingSphere* sphere;
	vector<BulletStruct*> bullets;

};