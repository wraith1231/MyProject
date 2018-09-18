#include "stdafx.h"
#include "ToonShading.h"

#include "../Viewer/Orthographic.h"

ToonShading::ToonShading(ExecuteValues* values)
	: Execute(values)
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	dirLight = new Shader(Shaders + L"DirLight.hlsl");

	ssaoShader = new Shader(Shaders + L"999_SSAO.hlsl");
	edgeShader = new Shader(Shaders + L"999_Edge.hlsl");
	aaShader = new Shader(Shaders + L"999_FXAA.hlsl");

	shadowRT = new RenderTarget((UINT)desc.Width, (UINT)desc.Height, DXGI_FORMAT_R16_FLOAT);

	normalRT = new RenderTarget((UINT)desc.Width, (UINT)desc.Height);
	depthRT = new RenderTarget((UINT)desc.Width, (UINT)desc.Height);
	diffuseRT = new RenderTarget((UINT)desc.Width, (UINT)desc.Height);

	lightRT = new RenderTarget((UINT)desc.Width, (UINT)desc.Height);

	ssaoRT = new RenderTarget((UINT)(desc.Width * 0.75f), (UINT)(desc.Height * 0.75f));

	edgeRT = new RenderTarget((UINT)desc.Width, (UINT)desc.Height);

	{
		Orthographic* projection = new Orthographic(-desc.Width * 0.5f, desc.Width * 0.5f, -desc.Height * 0.5f, desc.Height * 0.5f, 0.1f, 1000.0f);
		D3DXMATRIX p;
		projection->GetMatrix(&p);
		values->GlobalLight->SetLightProjection(p);
		SAFE_DELETE(projection);
	}
	
	buffer = new Buffer;
	buffer->Data.Near = values->Perspective->GetNearZ();
	buffer->Data.Far = values->Perspective->GetFarZ();
	lightBuffer = new TargetBuffer;

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

	ssaoBuffer = new SSAOBuffer();
}

ToonShading::~ToonShading()
{
	SAFE_DELETE(ssaoBuffer);
	SAFE_DELETE(dirBuffer);
	SAFE_DELETE(lightBuffer);
	SAFE_DELETE(buffer);

	SAFE_DELETE(aaShader);
	SAFE_DELETE(edgeShader);
	SAFE_DELETE(ssaoShader);

	SAFE_DELETE(edgeRT);
	SAFE_DELETE(ssaoRT);
	SAFE_DELETE(lightRT);
	SAFE_DELETE(normalRT);
	SAFE_DELETE(depthRT);
	SAFE_DELETE(diffuseRT);
	SAFE_DELETE(shadowRT);
}

void ToonShading::Update()
{
}

void ToonShading::ShadowRender()
{
	buffer->Data.Width = static_cast<float>(normalRT->GetWidth());
	buffer->Data.Height = static_cast<float>(normalRT->GetHeight());
	buffer->SetVSBuffer(11);
	buffer->SetPSBuffer(11);

	shadowRT->Set();
}

void ToonShading::PreRender()
{
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
	D3D::GetDC()->PSSetShaderResources(0, 1, &normalView);
	ID3D11ShaderResourceView* depthView = depthRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(1, 1, &depthView);
	ID3D11ShaderResourceView* diffuseView = diffuseRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(2, 1, &diffuseView);
	ID3D11ShaderResourceView* shadowView = shadowRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(3, 1, &shadowView);
	dirBuffer->SetPSBuffer(2);
	dirLight->Render();
	D3D::GetDC()->Draw(4, 0);

}

void ToonShading::SSAORender()
{
	Shader::ClearShader();

	ssaoRT->Clear();
	ID3D11RenderTargetView* rtv = ssaoRT->GetRTV();
	D3D::GetDC()->OMSetRenderTargets(1, &rtv, D3D::Get()->GetReadOnlyDSV());

	ID3D11ShaderResourceView* normalView = normalRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(0, 1, &normalView);
	ID3D11ShaderResourceView* depthView = depthRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(1, 1, &depthView);
	ID3D11ShaderResourceView* diffuseView = diffuseRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(2, 1, &diffuseView);

	ssaoShader->Render();
	ssaoBuffer->SetPSBuffer(2);
	D3D::GetDC()->IASetInputLayout(NULL);
	D3D::GetDC()->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	D3D::GetDC()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	D3D::GetDC()->Draw(4, 0);
}

void ToonShading::EdgeRender()
{
	Shader::ClearShader();

	edgeRT->Clear();
	ID3D11RenderTargetView* rtv = edgeRT->GetRTV();
	D3D::GetDC()->OMSetRenderTargets(1, &rtv, D3D::Get()->GetReadOnlyDSV());

	ID3D11ShaderResourceView* normalView = normalRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(0, 1, &normalView);
	ID3D11ShaderResourceView* depthView = depthRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(1, 1, &depthView);
	ID3D11ShaderResourceView* diffuseView = diffuseRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(2, 1, &diffuseView);
	ID3D11ShaderResourceView* lightView = lightRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(3, 1, &lightView);
	ID3D11ShaderResourceView* shadowView = shadowRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(4, 1, &shadowView);
	ID3D11ShaderResourceView* ssaoView = ssaoRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(5, 1, &ssaoView);

	lightBuffer->SetPSBuffer(3);

	edgeShader->Render();
	D3D::GetDC()->IASetInputLayout(NULL);
	D3D::GetDC()->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	D3D::GetDC()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	D3D::GetDC()->Draw(4, 0);
}

void ToonShading::AARender()
{
	ID3D11ShaderResourceView* view = edgeRT->GetSRV();
	D3D::GetDC()->PSSetShaderResources(0, 1, &view);

	aaShader->Render();
	D3D::GetDC()->IASetInputLayout(NULL);
	D3D::GetDC()->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	D3D::GetDC()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	D3D::GetDC()->Draw(4, 0);
}

void ToonShading::ImGuiRender()
{
	ImGui::Begin("Buffer");

	{
		ImGui::SliderInt("Buffer Render", (int*)&lightBuffer->Data.BufferRender, 0, 7);

		if (lightBuffer->Data.BufferRender == 6)
		{
			ImGui::Separator();
			ImGui::SliderInt("SSAO Sample", (int*)&ssaoBuffer->Data.Sample, 1, 30);
			ImGui::InputFloat("SSAO Intensity", &ssaoBuffer->Data.Intensity);
			ImGui::InputFloat("SSAO Scale", &ssaoBuffer->Data.Scale);
			ImGui::InputFloat("SSAO Bias", &ssaoBuffer->Data.Bias);
			ImGui::InputFloat("SSAO Radius", &ssaoBuffer->Data.Radius);
			ImGui::InputFloat("SSAO Max Distance", &ssaoBuffer->Data.MaxDistance);
			ImGui::InputFloat3("SSAO Moo3", ssaoBuffer->Data.Moo3);
		}
	}

	ImGui::End();
}

void ToonShading::ResizeScreen()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	buffer->Data.Near = values->Perspective->GetNearZ();
	buffer->Data.Far = values->Perspective->GetFarZ();

	shadowRT->Create((UINT)desc.Width, (UINT)desc.Height, DXGI_FORMAT_R16_FLOAT);

	normalRT->Create((UINT)desc.Width, (UINT)desc.Height);

	depthRT->Create((UINT)desc.Width, (UINT)desc.Height);

	diffuseRT->Create((UINT)desc.Width, (UINT)desc.Height);

	lightRT->Create((UINT)desc.Width, (UINT)desc.Height);

	edgeRT->Create((UINT)desc.Width, (UINT)desc.Height);

}
