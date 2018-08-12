#pragma once

class RenderTarget
{
public:
	RenderTarget(UINT width = 1280, UINT height = 720, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT);
	~RenderTarget();
	
	ID3D11RenderTargetView* GetRTV()
	{
		return rtv;
	}

	ID3D11ShaderResourceView* GetSRV()
	{
		return srv;
	}

	ID3D11Texture2D* GetTexture()
	{
		return texture;
	}

	UINT GetWidth() { return width; }
	UINT GetHeight() { return height; }

	void Set();

	void Create(UINT width = 1280, UINT height = 720, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT);
private:
	void Delete();

private:
	ID3D11Texture2D* texture;		//backbuffer 대신
	ID3D11RenderTargetView* rtv;	//texture랑 연동해서 변경결과를 저장할 친구
	ID3D11ShaderResourceView* srv;	//그걸 shader랑 연계하게 해주는 친구

	UINT width, height;
};