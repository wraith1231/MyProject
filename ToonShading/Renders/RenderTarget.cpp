#include "stdafx.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget(UINT width, UINT height, DXGI_FORMAT format)
	: texture(NULL), rtv(NULL), srv(NULL)
{	
	Create(width, height, format);
}

RenderTarget::~RenderTarget()
{
	Delete();
}

void RenderTarget::Set()
{
	D3D::Get()->SetRenderTarget(rtv);
	D3D::Get()->Clear(D3DXCOLOR(0, 1, 0, 0), rtv);
}

void RenderTarget::Create(UINT width, UINT height, DXGI_FORMAT format)
{
	Delete();

	D3DDesc desc;
	D3D::GetDesc(&desc);

	if (width <= 0 && height <= 0) return;

	if (width < 1)
		width = (UINT)desc.Width;
	if (height < 1)
		height = (UINT)desc.Height;

	this->width = width;
	this->height = height;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));

	//화면 사이즈에 맞춘거
	texDesc.Width = (UINT)width;
	texDesc.Height = (UINT)height;

	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	HRESULT hr;
	hr = D3D::GetDevice()->CreateTexture2D(&texDesc, NULL, &texture);
	assert(SUCCEEDED(hr));

	D3D11_RENDER_TARGET_VIEW_DESC renderDesc;
	ZeroMemory(&renderDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	renderDesc.Format = texDesc.Format;
	renderDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderDesc.Texture2D.MipSlice = 0;

	hr = D3D::GetDevice()->CreateRenderTargetView(texture, &renderDesc, &rtv);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	ZeroMemory(&viewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	viewDesc.Format = texDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels = 1;

	hr = D3D::GetDevice()->CreateShaderResourceView(texture, &viewDesc, &srv);
	assert(SUCCEEDED(hr));
}

void RenderTarget::Delete()
{
	SAFE_RELEASE(texture);
	SAFE_RELEASE(rtv);
	SAFE_RELEASE(srv);
}
