#include "stdafx.h"
#include "Bullet.h"

#include "../Bounding/BoundingSphere.h"
#include "../Bounding/BoundingBox.h"

Bullet::Bullet()
{
	sphere = new Objects::BoundingSphere(D3DXVECTOR3(0, 0, 0), 0.3f);
	sphere->SetColor(D3DXCOLOR(0, 1, 0, 1));
	sphere->SetVertex(3);
}

Bullet::~Bullet()
{
	for (BulletStruct* bullet : bullets)
		SAFE_DELETE(bullet);
}

void Bullet::AddBullet(D3DXVECTOR3 pos, D3DXVECTOR3 dir, float speed, float range, bool useGravity, D3DXVECTOR3 gravity, float gravityPower)
{
	BulletStruct* bullet = new BulletStruct();

	bullet->Start = pos;
	bullet->Position = pos;
	bullet->Direction = dir;

	bullet->UseGravity = useGravity;
	bullet->GravityDir = gravity;
	bullet->Gravity = -gravity * 0.01f;
	//bullet->GravitySpeed = D3DXVECTOR3(0, 0, 0);
	bullet->GravityPower = gravityPower;

	bullet->Speed = speed;
	bullet->Range = range;

	D3DXMatrixTranslation(&bullet->World, pos.x, pos.y, pos.z);

	bullets.push_back(bullet);
}

void Bullet::Update()
{
	float delta = Time::Delta();
	for (UINT i = 0; i < bullets.size();)
	{
		if (bullets[i]->UseGravity == true)
		{
			bullets[i]->Gravity += bullets[i]->GravityDir * bullets[i]->GravityPower * delta * delta;// *delta;

			D3DXVECTOR3 temp = bullets[i]->Direction * bullets[i]->Speed * delta + bullets[i]->Gravity * 0.5f;
			bullets[i]->Position += temp;

			temp = bullets[i]->Position - bullets[i]->Start;

			if (D3DXVec3Length(&temp) >= bullets[i]->Range)
			{
				BulletStruct* bullet = bullets[i];
				bullets.erase(bullets.begin() + i);
				SAFE_DELETE(bullet);

				continue;
			}
			else
			{
				D3DXMatrixTranslation(&bullets[i]->World, bullets[i]->Position.x, bullets[i]->Position.y, bullets[i]->Position.z);
				i++;
			}
		}
		else
		{
			bullets[i]->Position += bullets[i]->Direction * bullets[i]->Speed * delta;
			D3DXVECTOR3 temp = bullets[i]->Position - bullets[i]->Start;
			if (D3DXVec3Length(&temp) >= bullets[i]->Range)
			{
				BulletStruct* bullet = bullets[i];
				bullets.erase(bullets.begin() + i);
				SAFE_DELETE(bullet);
				continue;
			}
			else
			{
				D3DXMatrixTranslation(&bullets[i]->World, bullets[i]->Position.x, bullets[i]->Position.y, bullets[i]->Position.z);
				i++;
			}
		}
	}
}

void Bullet::Render()
{
	for (BulletStruct* bullet : bullets)
	{
		sphere->SetWorld(bullet->World);
		sphere->Render();
	}
}

bool Bullet::CheckIntersect(Objects::BoundingBox * box)
{
	for (BulletStruct* bullet : bullets)
	{
		float single;
		float radius = sphere->GetRadius();
		D3DXVECTOR3 vec(0, 0, 0);
		D3DXVECTOR3 center = bullet->Position;
		vec = Math::Clamp(center, box->GetMin(), box->GetMax());

		single = Math::DistanceSquared(center, vec);

		if (single <= radius * radius)
			return true;
	}

	return false;
}
