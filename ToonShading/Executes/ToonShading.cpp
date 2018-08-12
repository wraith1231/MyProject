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

	edgeShader = new Shader(Shaders + L"999_Edge.hlsl");
	aaShader = new Shader(Shaders + L"999_FXAA.hlsl");
	for (Material* material : edgeModel->Materials())
		material->SetShader(edgeShader);
	for (Material* material : aaModel->Materials())
		material->SetShader(aaShader);

	worldBuffer = new WorldBuffer();

	D3DXMatrixIdentity(&view);

	normalRT = new RenderTarget(desc.Width, desc.Height);
	depthRT = new RenderTarget(desc.Width, desc.Height);
	realRT = new RenderTarget(desc.Width, desc.Height);
	lightRT = new RenderTarget(desc.Width, desc.Height);
	AART = new RenderTarget(desc.Width, desc.Height);

	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		projection = new Orthographic(0, desc.Width, 0, desc.Height);
	}
	
	buffer = new Buffer;
	buffer->Data.Near = values->Perspective->GetNearZ();
	buffer->Data.Far = values->Perspective->GetFarZ();

	xplus = yplus = zplus = 0.0f;

	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, desc.Width, desc.Height, 1.0f);
	D3DXMatrixRotationY(&R, (float)D3DX_PI);
	D3DXMatrixTranslation(&T, desc.Width / 2, desc.Height / 2, 0.5f);

	edgeModel->Bone(0)->Transform(S*R*T);

	D3DXMatrixTranslation(&T, desc.Width / 2, desc.Height / 2, 0.4f);
	aaModel->Bone(0)->Transform(S*R*T);
}

ToonShading::~ToonShading()
{
	SAFE_DELETE(aaShader);
	SAFE_DELETE(edgeShader);

	SAFE_DELETE(aaModel);
	SAFE_DELETE(edgeModel);

	SAFE_DELETE(AART);
	SAFE_DELETE(lightRT);
	SAFE_DELETE(normalRT);
	SAFE_DELETE(depthRT);
	SAFE_DELETE(realRT);

	SAFE_DELETE(projection);
}

void ToonShading::Update()
{
}

void ToonShading::NormalRender()
{
	normalRT->Set();

}

void ToonShading::DepthRender()
{
	depthRT->Set();
}

void ToonShading::DiffuseRender()
{
	realRT->Set();
}

void ToonShading::LightRender()
{
	lightRT->Set();
}

void ToonShading::EdgeRender()
{
	AART->Set();

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
	edgeModel->TransformsCopy();
	edgeModel->Render();
}

void ToonShading::AARender()
{
	values->ViewProjection->SetView(view);
	
	D3DXMATRIX pro;
	projection->GetMatrix(&pro);
	
	values->ViewProjection->SetProjection(pro);
	values->ViewProjection->SetVSBuffer(0);

	ID3D11ShaderResourceView* view = AART->GetSRV();
	D3D::GetDC()->PSSetShaderResources(5, 1, &view);

	buffer->Data.Width = static_cast<float>(AART->GetWidth());
	buffer->Data.Height = static_cast<float>(AART->GetHeight());
	buffer->SetPSBuffer(2);
	buffer->SetVSBuffer(2);
	aaModel->TransformsCopy();
	aaModel->Render();
}

void ToonShading::ImGuiRender()
{

}

void ToonShading::ResizeScreen()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	projection->Set(0, desc.Width, 0, desc.Height);//, -1.0f);

	buffer->Data.Near = values->Perspective->GetNearZ();
	buffer->Data.Far = values->Perspective->GetFarZ();

	normalRT->Create(desc.Width, desc.Height);

	depthRT->Create(desc.Width, desc.Height);

	realRT->Create(desc.Width, desc.Height);

	lightRT->Create(desc.Width, desc.Height);

	AART->Create(desc.Width, desc.Height);

}
