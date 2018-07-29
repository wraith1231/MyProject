#include "stdafx.h"
#include "Bullet.h"

#include "../Bounding/BoundingSphere.h"
#include "../Bounding/BoundingBox.h"

Bullet::Bullet()
{

	model = new Model();
	model->ReadMaterial(Models + L"Bullets/Bullet.material");
	model->ReadMesh(Models + L"Bullets/Bullet.mesh");
	D3DXVECTOR3 max, min;
	model->CheckMaxMinVer(max, min);
	float rad = max.y - min.y;

	shader = new Shader(Shaders + L"999_Mesh.hlsl");
	shader2 = new Shader(Shaders + L"999_Mesh.hlsl", "VS_Normal", "PS_Normal");
	shader3 = new Shader(Shaders + L"999_Mesh.hlsl", "VS_Depth", "PS_Depth");

	sphere = new Objects::BoundingSphere(D3DXVECTOR3(0, 0, 0), rad);
	//sphere->SetColor(D3DXCOLOR(0, 1, 0, 1));
	//sphere->SetVertex(3);
}

Bullet::~Bullet()
{
	for (BulletStruct* bullet : bullets)
		SAFE_DELETE(bullet);
	SAFE_DELETE(sphere);
	
	SAFE_DELETE(shader);
	SAFE_DELETE(shader2);
	SAFE_DELETE(shader3);

	SAFE_DELETE(model);
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

void Bullet::DeleteBullet(UINT num)
{
	if (num >= bullets.size()) return;

	SAFE_DELETE(bullets[num]);
	bullets.erase(bullets.begin() + num);
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

void Bullet::PreRender()
{
	for (Material* material : model->Materials())
		material->SetShader(shader2);

	for (BulletStruct* bullet : bullets)
	{
		for (ModelMesh* mesh : model->Meshes())
		{
			mesh->SetWorld(bullet->World);
			mesh->Render();
		}
		//model->SetWorld(bullet->World);
		//model->Render();
	}
}

void Bullet::PreRender2()
{
	for (Material* material : model->Materials())
		material->SetShader(shader3);

	for (BulletStruct* bullet : bullets)
	{
		for (ModelMesh* mesh : model->Meshes())
		{
			mesh->SetWorld(bullet->World);
			mesh->Render();
		}
		//model->SetWorld(bullet->World);
		//model->Render();
	}
}

void Bullet::Render()
{
	for (Material* material : model->Materials())
		material->SetShader(shader);

	for (BulletStruct* bullet : bullets)
	{
		for (ModelMesh* mesh : model->Meshes())
		{
			mesh->SetWorld(bullet->World);
			mesh->Render();
		}
		//model->SetWorld(bullet->World);
		//model->Render();
	}
}

bool Bullet::CheckIntersect(Objects::BoundingBox * box, UINT& num)
{
	for (UINT i = 0; i < bullets.size(); i++)
	{
		float single;
		float radius = sphere->GetRadius();
		D3DXVECTOR3 vec(0, 0, 0);
		D3DXVECTOR3 center = bullets[i]->Position;

		D3DXVECTOR3 max, min;
		max = Math::Max(box->GetMax(), box->GetMin());
		min = Math::Min(box->GetMax(), box->GetMin());
		
		vec = Math::Clamp(center, min, max);

		single = Math::DistanceSquared(center, vec);

		if (single <= radius * radius)
		{
			num = i;
			return true;
		}
	}

	return false;
}

void Bullet::BulletClear()
{
	for (BulletStruct* bullet : bullets)
		SAFE_DELETE(bullet);
	bullets.clear();
}

UINT Bullet::BulletQuan()
{
	return bullets.size();
}
