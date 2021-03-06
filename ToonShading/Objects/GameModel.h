#pragma once
#include "GameRender.h"

struct ExecuteValues;
class GameModel : public GameRender
{
private:
	class BoneBuffer : public ShaderBuffer
	{
	public:
		BoneBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			for (int i = 0; i < 256; i++)
				D3DXMatrixIdentity(&Data.Bones[i]);
		}

		void SetBones(D3DXMATRIX* m, UINT count)
		{
			memcpy(Data.Bones, m, sizeof(D3DXMATRIX) * count);

			for (UINT i = 0; i < count; i++)
				D3DXMatrixTranspose(&Data.Bones[i], &Data.Bones[i]);
		}

	private:
		struct Struct
		{
			D3DXMATRIX Bones[256];
		} Data;
	};

	class RenderBuffer : public ShaderBuffer
	{
	public:
		RenderBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.BoneNumber = 0;
		}

	public:
		struct Struct
		{
			int BoneNumber;

			float Padding[3];
		} Data;
	};

public:
	GameModel(wstring materialFile, wstring meshFile, ExecuteValues* value = NULL);
	virtual ~GameModel();

	virtual void Update();

	virtual void ShadowRender();
	virtual void PreRender();
	virtual void ImGuiRender();

	void Rotate(D3DXVECTOR2 amount);

	void Velocity(D3DXVECTOR3& vec);
	D3DXVECTOR3 Velocity();

	D3DXVECTOR3 CalcVelocity(D3DXVECTOR3& velocity);

	void AddPosition(D3DXVECTOR3& vec);
	void SetPosition(D3DXVECTOR3& vec);

protected:	//private function
	void CalcPosition();

protected:	//private valuable
	Shader* shader;
	Shader* shadowShader;

	Model* model;

	D3DXVECTOR3 velocity;
	D3DXMATRIX rotateMatrix;

	vector<D3DXMATRIX> boneTransforms;

protected:	//private buffer
	BoneBuffer* boneBuffer;
	RenderBuffer* renderBuffer;
};