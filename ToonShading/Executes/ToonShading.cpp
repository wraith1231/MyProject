#include "stdafx.h"
#include "ToonShading.h"

#include "../Viewer/Orthographic.h"

ToonShading::ToonShading(ExecuteValues* values)
	: Execute(values)
{
	model = new Model();
	model->ReadMaterial(Models + L"Mesh/Quad.material");
	model->ReadMesh(Models + L"Mesh/Quad.mesh");

	shader = new Shader(Shaders + L"999_Edge.hlsl");
	for (Material* material : model->Materials())
		material->SetShader(shader);

	worldBuffer = new WorldBuffer();

	D3DXMatrixIdentity(&view);

	normalRT = new RenderTarget();
	depthRT = new RenderTarget();
	realRT = new RenderTarget();

	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		projection = new Orthographic(0, desc.Width, 0, desc.Height);
	}
	
	buffer = new Buffer;
	buffer->Data.Near = values->Perspective->GetNearZ();
	buffer->Data.Far = values->Perspective->GetFarZ();
}

ToonShading::~ToonShading()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(model);
	SAFE_DELETE(normalRT);
	SAFE_DELETE(depthRT);
	SAFE_DELETE(realRT);
	SAFE_DELETE(projection);
}

void ToonShading::Update()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	D3DXMATRIX S, R,T;
	D3DXMatrixScaling(&S, desc.Width, desc.Height, 1.0f);
	D3DXMatrixRotationY(&R, (float)D3DX_PI);
	D3DXMatrixTranslation(&T, desc.Width/2, desc.Height / 2, 0.1f);

	model->Bone(0)->Transform(S*R*T);
}

void ToonShading::PreRender()
{
	normalRT->Set();

}

void ToonShading::PreRender2()
{
	depthRT->Set();
}

void ToonShading::Render()
{
	realRT->Set();
}

void ToonShading::PostRender()
{
	values->ViewProjection->SetView(view);
	
	D3DXMATRIX pro;
	projection->GetMatrix(&pro);
	
	values->ViewProjection->SetProjection(pro);
	values->ViewProjection->SetVSBuffer(0);

	ID3D11ShaderResourceView* normalView = normalRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(5, 1, &normalView);
	ID3D11ShaderResourceView* depthView = depthRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(6, 1, &depthView);
	ID3D11ShaderResourceView* realView = realRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(7, 1, &realView);

	buffer->Data.Width = static_cast<float>(normalRT->GetWidth());
	buffer->Data.Height = static_cast<float>(normalRT->GetHeight());
	buffer->SetPSBuffer(2);
	model->Render();
}

void ToonShading::ImGuiRender()
{
}

void ToonShading::ResizeScreen()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	projection->Set(0, desc.Width, 0, desc.Height);//, -1.0f);

	normalRT->Create();

	depthRT->Create();

	realRT->Create();
}
