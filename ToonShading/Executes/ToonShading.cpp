#include "stdafx.h"
#include "ToonShading.h"

#include "../Viewer/Orthographic.h"

ToonShading::ToonShading(ExecuteValues* values)
	: Execute(values)
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	edgeModel = new Model();
	edgeModel->ReadMaterial(Models + L"Mesh/Quad.material");
	edgeModel->ReadMesh(Models + L"Mesh/Quad.mesh");

	aaModel = new Model();
	aaModel->ReadMaterial(Models + L"Mesh/Quad.material");
	aaModel->ReadMesh(Models + L"Mesh/Quad.mesh");

	dirLight = new Shader(Shaders + L"DirLight.hlsl");

	edgeShader = new Shader(Shaders + L"999_Edge.hlsl");
	aaShader = new Shader(Shaders + L"999_FXAA.hlsl");

	for (Material* material : edgeModel->Materials())
		material->SetShader(edgeShader);
	for (Material* material : aaModel->Materials())
		material->SetShader(aaShader);

	worldBuffer = new WorldBuffer();

	D3DXMatrixIdentity(&view);

	shadowRT = new RenderTarget((UINT)desc.Width, (UINT)desc.Height, DXGI_FORMAT_R16_FLOAT);
	normalRT = new RenderTarget((UINT)desc.Width, (UINT)desc.Height);// , DXGI_FORMAT_R32G32_FLOAT);
	depthRT = new RenderTarget((UINT)desc.Width, (UINT)desc.Height);
	diffuseRT = new RenderTarget((UINT)desc.Width, (UINT)desc.Height);
	lightRT = new RenderTarget((UINT)desc.Width, (UINT)desc.Height);
	AART = new RenderTarget((UINT)desc.Width, (UINT)desc.Height);

	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		projection = new Orthographic(0, desc.Width, 0, desc.Height);
	}
	
	buffer = new Buffer;
	buffer->Data.Near = values->Perspective->GetNearZ();
	buffer->Data.Far = values->Perspective->GetFarZ();
	lightBuffer = new LightBuffer;

	xplus = yplus = zplus = 0.0f;

	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, desc.Width, desc.Height, 1.0f);
	D3DXMatrixRotationY(&R, (float)D3DX_PI);
	D3DXMatrixTranslation(&T, desc.Width / 2, desc.Height / 2, 0.5f);

	edgeModel->Bone(0)->Transform(S*R*T); 

	D3DXMatrixTranslation(&T, desc.Width / 2, desc.Height / 2, 0.4f);
	aaModel->Bone(0)->Transform(S*R*T);

	{
		D3D11_DEPTH_STENCIL_DESC desc;
		desc.DepthEnable = TRUE;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		desc.StencilEnable = TRUE;
		desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		D3D11_DEPTH_STENCILOP_DESC op = { D3D11_STENCIL_OP_KEEP,D3D11_STENCIL_OP_KEEP ,D3D11_STENCIL_OP_KEEP , D3D11_COMPARISON_EQUAL };
		desc.FrontFace = op;
		desc.BackFace = op;
		HRESULT hr = D3D::GetDevice()->CreateDepthStencilState(&desc, &writeLessStencilMask);
		assert(SUCCEEDED(hr));
	}
	dirBuffer = new DirBuffer;
}

ToonShading::~ToonShading()
{
	SAFE_DELETE(lightBuffer);
	SAFE_DELETE(buffer);

	SAFE_DELETE(aaShader);
	SAFE_DELETE(edgeShader);

	SAFE_DELETE(aaModel);
	SAFE_DELETE(edgeModel);

	SAFE_DELETE(AART);
	SAFE_DELETE(lightRT);
	SAFE_DELETE(normalRT);
	SAFE_DELETE(depthRT);
	SAFE_DELETE(diffuseRT);
	SAFE_DELETE(shadowRT);

	SAFE_DELETE(projection);
}

void ToonShading::Update()
{
}

void ToonShading::ShadowRender()
{
	shadowRT->Set();
}

void ToonShading::PreRender()
{
	buffer->SetPSBuffer(9);
	buffer->SetVSBuffer(9);

	normalRT->Clear();
	depthRT->Clear();
	diffuseRT->Clear();
	ID3D11RenderTargetView* rtv[3] = { normalRT->GetRTV(), depthRT->GetRTV(), diffuseRT->GetRTV() };
	D3D::GetDC()->OMSetRenderTargets(3, rtv, D3D::Get()->GetDSV());
	D3D::GetDC()->OMSetDepthStencilState(D3D::Get()->GetDSS(), 1);
}

void ToonShading::LightRender()
{
	lightRT->Clear();
	ID3D11RenderTargetView* rtv = lightRT->GetRTV();
	D3D::GetDC()->OMSetRenderTargets(1, &rtv, D3D::Get()->GetReadOnlyDSV());

	dirBuffer->Data.DirColor = D3DXVECTOR3(values->GlobalLight->Data.Color.r, values->GlobalLight->Data.Color.g, values->GlobalLight->Data.Color.b);
	dirBuffer->Data.DirToLight = -values->GlobalLight->Data.Direction;

	//directional;
	D3D::GetDC()->IASetInputLayout(NULL);
	D3D::GetDC()->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	D3D::GetDC()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11ShaderResourceView* normalView = normalRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(5, 1, &normalView);
	ID3D11ShaderResourceView* depthView = depthRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(6, 1, &depthView);
	ID3D11ShaderResourceView* diffuseView = diffuseRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(7, 1, &diffuseView);
	dirBuffer->SetPSBuffer(2);
	dirLight->Render();
	D3D::GetDC()->Draw(4, 0);
}

void ToonShading::EdgeRender()
{
	AART->Set();

	values->ViewProjection->SetView(view);
	
	D3DXMATRIX pro;
	projection->GetMatrix(&pro);
	
	values->ViewProjection->SetProjection(pro);
	values->ViewProjection->SetVSBuffer(10);

	ID3D11ShaderResourceView* normalView = normalRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(5, 1, &normalView);
	ID3D11ShaderResourceView* depthView = depthRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(6, 1, &depthView);
	ID3D11ShaderResourceView* diffuseView = diffuseRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(7, 1, &diffuseView);
	ID3D11ShaderResourceView* lightView = lightRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(8, 1, &lightView);

	buffer->Data.Width = static_cast<float>(normalRT->GetWidth());
	buffer->Data.Height = static_cast<float>(normalRT->GetHeight());
	lightBuffer->SetPSBuffer(3);
	edgeModel->TransformsCopy();
	edgeModel->Render();
}

void ToonShading::AARender()
{
	values->ViewProjection->SetView(view);
	
	D3DXMATRIX pro;
	projection->GetMatrix(&pro);
	
	values->ViewProjection->SetProjection(pro);
	values->ViewProjection->SetVSBuffer(10);

	ID3D11ShaderResourceView* view = AART->GetSRV();
	D3D::GetDC()->PSSetShaderResources(5, 1, &view);

	buffer->Data.Width = static_cast<float>(AART->GetWidth());
	buffer->Data.Height = static_cast<float>(AART->GetHeight());
	aaModel->TransformsCopy();
	aaModel->Render();
}

void ToonShading::ImGuiRender()
{
	ImGui::Begin("Buffer");

	{
		ImGui::SliderInt("Buffer Render", (int*)&lightBuffer->Data.BufferRender, 0, 5);
	}

	ImGui::End();
}

void ToonShading::ResizeScreen()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	projection->Set(0, desc.Width, 0, desc.Height);//, -1.0f);

	buffer->Data.Near = values->Perspective->GetNearZ();
	buffer->Data.Far = values->Perspective->GetFarZ();

	normalRT->Create((UINT)desc.Width, (UINT)desc.Height);

	depthRT->Create((UINT)desc.Width, (UINT)desc.Height);

	diffuseRT->Create((UINT)desc.Width, (UINT)desc.Height);

	lightRT->Create((UINT)desc.Width, (UINT)desc.Height);

	AART->Create((UINT)desc.Width, (UINT)desc.Height);

}
