#pragma once
#include "stdafx.h"

class WorldBuffer : public ShaderBuffer
{
public:
	WorldBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		D3DXMatrixIdentity(&Data.World);
		//Data.Camera = D3DXVECTOR3(0, 0, 0);
	}

	void SetMatrix(D3DXMATRIX mat)
	{
		Data.World = mat;
		D3DXMatrixTranspose(&Data.World, &Data.World);
	}
	//void SetCamera(D3DXVECTOR3 cam)
	//{
	//	Data.Camera = cam;
	//}

	D3DXMATRIX GetMatrix()
	{
		D3DXMATRIX ret = Data.World;
		D3DXMatrixTranspose(&ret, &ret);
		return ret;
	}

	struct Struct
	{
		D3DXMATRIX World;
		//D3DXVECTOR3 Camera;
		//float Padding;
	};

private:
	Struct Data;
};

class ViewProjectionBuffer : public ShaderBuffer
{
public:
	ViewProjectionBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		D3DXMatrixIdentity(&Data.View);
		D3DXMatrixIdentity(&Data.Projection);
		D3DXMatrixIdentity(&Data.ViewInverse);

		Data.ElapsedTime = 0.0f;
		Data.RunningTime = 0.0f;
	}

	void SetView(D3DXMATRIX& mat)
	{
		Data.View = mat;
		D3DXMatrixInverse(&Data.ViewInverse, NULL, &Data.View);

		D3DXMatrixTranspose(&Data.View, &Data.View);
		D3DXMatrixTranspose(&Data.ViewInverse, &Data.ViewInverse);
	}

	void SetProjection(D3DXMATRIX& mat)
	{
		Data.Projection = mat;
		D3DXMatrixTranspose(&Data.Projection, &Data.Projection);
	}

	void GetView(D3DXMATRIX& mat)
	{
		mat = Data.View;
	}

	void GetProjection(D3DXMATRIX& mat)
	{
		mat = Data.Projection;
	}

	void AddTime(float time)
	{
		Data.ElapsedTime = time;
		Data.RunningTime += time;

		if (Data.RunningTime >= 10000.0f)
			Data.RunningTime = 0.0f;
	}

	struct Struct
	{
		D3DXMATRIX View;
		D3DXMATRIX Projection;
		D3DXMATRIX ViewInverse;

		float ElapsedTime;
		float RunningTime;
		float Padding[2];
	};

private:
	Struct Data;
};
/*
class LightBuffer : public ShaderBuffer
{
public:
	LightBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		Data.Direction = D3DXVECTOR3(-1, -1, -1);
	}

	struct Struct
	{
		D3DXVECTOR3 Direction;

		float Padding;
	};

	Struct Data;
};
*/
class LightBuffer : public ShaderBuffer
{
public:
	LightBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		Data.Direction = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
		Data.Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		Data.Overcast = 1.0f;
		Data.Intensity = 0.2f;
		Data.LightPosition = D3DXVECTOR3(0, 0, 0);

		D3DXMatrixIdentity(&Data.LightView);
		D3DXMatrixIdentity(&Data.LightProjection);
	}

	void SetLightView(D3DXMATRIX mat)
	{
		D3DXMatrixTranspose(&Data.LightView, &mat);
	}

	void SetLightProjection(D3DXMATRIX mat)
	{
		D3DXMatrixTranspose(&Data.LightProjection, &mat);
	}

	struct Struct
	{
		D3DXVECTOR3 Direction;
		float Padding;

		D3DXCOLOR Color;

		float Overcast;
		float Intensity;	//±¤¿ø °­µµ
		float Padding2[2];

		D3DXVECTOR3 LightPosition;
		float Padding3;

		D3DXMATRIX LightView;
		D3DXMATRIX LightProjection;
	};

	Struct Data;
};