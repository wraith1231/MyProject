#pragma once

class Water
{
private:
	class WaterBuffer : public ShaderBuffer
	{
	public:

		struct Struct
		{
			D3DXVECTOR3 WaterHeight;

		};
	};

public:
	Water(UINT width, UINT height);
	~Water();

private:

};