#pragma once

//=======================================================================================

struct Vertex
{
	Vertex()
		: position(0, 0, 0) {}

	D3DXVECTOR3 position;
};

//=======================================================================================

struct VertexColor
{
	VertexColor()
		: position(0, 0, 0)
		, color(0, 0, 0, 1) {}

	D3DXVECTOR3	position;
	D3DXCOLOR color;
};

//=======================================================================================

struct VertexColorNormal
{
	VertexColorNormal()
		: position(0, 0, 0)
		, color(0, 0, 0, 1)
		, normal(0, 0, 0) {}

	D3DXVECTOR3 position;
	D3DXCOLOR color;
	D3DXVECTOR3 normal;
};

//=======================================================================================

struct VertexTexture
{
	VertexTexture()
		: position(0, 0, 0)
		, uv(0, 0) {}

	D3DXVECTOR3	position;
	D3DXVECTOR2	uv;
};

//=======================================================================================

struct VertexTextureNormal
{
	VertexTextureNormal()
		: position(0, 0, 0)
		, uv(0, 0)
		, normal(0, 0, 0) {}

	D3DXVECTOR3 position;
	D3DXVECTOR2 uv;
	D3DXVECTOR3 normal;
};

//=======================================================================================

struct VertexColorTextureNormal
{
	VertexColorTextureNormal()
		: position(0, 0, 0)
		, color(0, 0, 0, 1)
		, uv(0, 0)
		, normal(0, 0, 0) {}

	D3DXVECTOR3 position;
	D3DXCOLOR color;
	D3DXVECTOR2 uv;
	D3DXVECTOR3 normal;
};

//=======================================================================================

struct VertexTextureNormalBlend
{
	VertexTextureNormalBlend()
		: position(0, 0, 0)
		, uv(0, 0)
		, normal(0, 0, 0)
		, blendIndices(0, 0, 0, 0)
		, blendWeights(0, 0, 0, 0) {}

	D3DXVECTOR3 position;
	D3DXVECTOR2 uv;
	D3DXVECTOR3 normal;
	D3DXVECTOR4 blendIndices;
	D3DXVECTOR4 blendWeights;
};

//=======================================================================================

struct VertexTextureNormalTangent
{
	VertexTextureNormalTangent()
		: position(0, 0, 0)
		, uv(0, 0)
		, normal(0, 0, 0)
		, tangent(0, 0, 0)
	{}

	D3DXVECTOR3	position;
	D3DXVECTOR2	uv;
	D3DXVECTOR3	normal;
	D3DXVECTOR3 tangent;
};


struct VertexTextureColor2
{
	VertexTextureColor2()
		: position(0, 0, 0)
		, uv(0, 0)
		, color(0, 0, 0, 1) 
		, color2(0, 0, 0, 1)
	{
		
	}

	D3DXVECTOR3 position;
	D3DXVECTOR2 uv;
	D3DXCOLOR color;
	D3DXCOLOR color2;
};

struct VertexTextureNormalColor
{
	VertexTextureNormalColor()
		: position(0, 0, 0)
		, uv(0, 0)
		, normal(0, 0, 0)
		, color(0, 0, 0, 1)
	{

	}

	D3DXVECTOR3 position;
	D3DXVECTOR2 uv;
	D3DXVECTOR3 normal;
	D3DXCOLOR color;
};