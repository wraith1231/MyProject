#include "stdafx.h"
#include "ToonShading2.h"

#include "../Viewer/Orthographic.h"

ToonShading2::ToonShading2(ExecuteValues* values)
	: Execute(values)
{
	model = new Model();
	model->ReadMaterial(Models + L"Mesh/Quad.material");
	model->ReadMesh(Models + L"Mesh/Quad.mesh");

	shader = new Shader(Shaders + L"999_Edge2.hlsl");
	for (Material* material : model->Materials())
		material->SetShader(shader);

	worldBuffer = new WorldBuffer();

	D3DXMatrixIdentity(&view);

	ndRT = new RenderTarget();
	realRT = new RenderTarget();

	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		projection = new Orthographic(0, desc.Width, 0, desc.Height);
	}
	
	buffer = new Buffer;
}

ToonShading2::~ToonShading2()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(model);
	SAFE_DELETE(ndRT);
	SAFE_DELETE(realRT);
	SAFE_DELETE(projection);
}

void ToonShading2::Update()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	D3DXMATRIX S, R,T;
	D3DXMatrixScaling(&S, desc.Width, desc.Height, 1.0f);
	D3DXMatrixRotationY(&R, (float)D3DX_PI);
	D3DXMatrixTranslation(&T, desc.Width/2, desc.Height / 2, 0.1f);

	model->Bone(0)->Transform(S*R*T);
}

void ToonShading2::PreRender()
{
	ndRT->Set();

}

void ToonShading2::PreRender2()
{

}

void ToonShading2::Render()
{
	realRT->Set();
}

void ToonShading2::PostRender()
{
	values->ViewProjection->SetView(view);
	
	D3DXMATRIX pro;
	projection->GetMatrix(&pro);
	
	values->ViewProjection->SetProjection(pro);
	values->ViewProjection->SetVSBuffer(0);

	ID3D11ShaderResourceView* normalView = ndRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(5, 1, &normalView);
	ID3D11ShaderResourceView* realView = realRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(6, 1, &realView);

	buffer->Data.Width = static_cast<float>(ndRT->GetWidth());
	buffer->Data.Height = static_cast<float>(ndRT->GetHeight());
	buffer->SetPSBuffer(2);
	model->Render();
}

void ToonShading2::ImGuiRender()
{
}

void ToonShading2::ResizeScreen()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	projection->Set(0, desc.Width, 0, desc.Height);//, -1.0f);

	ndRT->Create();

	realRT->Create();
}
