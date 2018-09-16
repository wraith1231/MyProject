#include "stdafx.h"
#include "CapsuleLight.h"

#include "../Bounding/BoundingBox.h"
#include "../Bounding/ObjectsRay.h"
#include "../Bounding/BoundingSphere.h"

#include "../Objects/MeshSphere.h"

CapsuleLight::CapsuleLight(ExecuteValues* values)
	: values(values)
{
	buffer = new Buffer();

	D3DXVECTOR3 max, min;
	max = D3DXVECTOR3(0.5, 0.5, 0.5);
	min = -max;

	box = new Objects::BoundingBox(max, min);

	psShader = new Shader(Shaders + L"CapsuleLight.hlsl");
	psShader->CreateHullShader();
	psShader->CreateDomainShader();

	{
		D3D11_BLEND_DESC desc;

		desc.AlphaToCoverageEnable = FALSE;
		desc.IndependentBlendEnable = FALSE;

		D3D11_RENDER_TARGET_BLEND_DESC def =
		{
			TRUE,
			D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
			D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
			D3D11_COLOR_WRITE_ENABLE_ALL,
		};
		for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
			desc.RenderTarget[i] = def;
		States::CreateBlend(&desc, &blend);
	}
	{
		D3D11_RASTERIZER_DESC desc =
		{
			D3D11_FILL_SOLID,
			D3D11_CULL_FRONT,
			FALSE,
			D3D11_DEFAULT_DEPTH_BIAS,
			D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
			D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
			FALSE,
			FALSE,
			FALSE,
			FALSE
		};
		desc.CullMode = D3D11_CULL_FRONT;
		D3D::GetDevice()->CreateRasterizerState(&desc, &rasterize);
	}

	dsBuffer = new DSBuffer();
}

CapsuleLight::~CapsuleLight()
{
	SAFE_RELEASE(rasterize);
	SAFE_RELEASE(blend);

	SAFE_DELETE(dsBuffer);
	SAFE_DELETE(buffer);

	SAFE_DELETE(psShader);

	SAFE_DELETE(box);
}

UINT CapsuleLight::AddCapsuleLight(D3DXVECTOR3 Position, D3DXVECTOR3 Color, D3DXVECTOR3 Dir, float RangeRcp, float Length, float HalfSeg, float Ran, bool add)
{
	CapsuleLightSave temp;
	temp.Position = Position;
	if (add == true)
		temp.Position.y += 2.0f;
	temp.Color = Color;
	temp.Direction = Dir;
	temp.RangeRcp = RangeRcp;
	temp.Range = Ran;
	temp.Length = Length;
	temp.HalfSegmentation = HalfSeg;

	lights.push_back(temp);

	return lights.size() - 1;
}

UINT CapsuleLight::CapsuleLightSize()
{
	return lights.size();
}

void CapsuleLight::PreRender()
{
	buffer->SetPSBuffer(10);

	if (lightSelect == true)
	{
		box->Update(lights[selectNum].Position);
		box->SetColor(D3DXCOLOR(lights[selectNum].Color.x, lights[selectNum].Color.y, lights[selectNum].Color.z, 1.0f));
		box->Render();
	}
}

void CapsuleLight::PreRender(bool val)
{
	buffer->SetPSBuffer(10);

	if (val == true)
	{
		for (size_t i = 0; i < lights.size(); i++)
		{
			box->Update(lights[i].Position);
			box->SetColor(D3DXCOLOR(lights[i].Color.x, lights[i].Color.y, lights[i].Color.z, 1.0f));
			box->Render();
		}
	}
}

void CapsuleLight::LightRender()
{
	ID3D11BlendState* prevBlen;
	FLOAT prevFactor[4];
	UINT prevMask;
	D3D::GetDC()->OMGetBlendState(&prevBlen, prevFactor, &prevMask);
	D3D::GetDC()->OMSetBlendState(blend, prevFactor, prevMask);

	ID3D11RasterizerState* prevRaster;
	D3D::GetDC()->RSGetState(&prevRaster);
	D3D::GetDC()->RSSetState(rasterize);

	for (CapsuleLightSave light : lights)
	{
		D3DXVECTOR3 temp = light.Position + light.Direction * light.Length * 0.5f;

		if (values->ViewFrustum->CheckSphere(temp.x, temp.y, temp.z, light.Length / 2) == false) continue;

		dsBuffer->Data.HalfSegmentation = light.HalfSegmentation;
		dsBuffer->Data.Range = light.Range;

		buffer->Data.Position = light.Position;
		buffer->Data.Color = light.Color;
		buffer->Data.RangeRcp = 1.0f / light.Range;
		buffer->Data.Direction = -light.Direction;
		buffer->Data.Length = light.Length;

		D3D::GetDC()->IASetInputLayout(NULL);
		D3D::GetDC()->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
		D3D::GetDC()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST);

		buffer->SetPSBuffer(2);
		dsBuffer->SetDSBuffer(2);
		psShader->Render();
		D3D::GetDC()->Draw(1, 0);
	}

	D3D::GetDC()->RSSetState(prevRaster);
	D3D::GetDC()->OMSetBlendState(prevBlen, prevFactor, prevMask);

	Shader::ClearShader();
}

void CapsuleLight::ImGuiRender()
{
	if (lightSelect == true)
	{
		ImGui::Begin("Capsule Light");

		{
			ImGui::Text("Count : %d", lights.size());
			bool b = true;
			ImGui::Checkbox("Use", &b);
			if (b == false)
			{
				lights.erase(lights.begin() + selectNum);
				selectNum = 0;
				lightSelect = false;

				ImGui::End();
				return;
			}

			ImGui::InputFloat3("Position", lights[selectNum].Position);
			ImGui::ColorEdit3("Color", lights[selectNum].Color);
			ImGui::SliderFloat3("Direction", lights[selectNum].Direction, -1.0f, 1.0f);
			D3DXVec3Normalize(&lights[selectNum].Direction, &lights[selectNum].Direction);

			ImGui::InputFloat("Range", &lights[selectNum].Range);
			ImGui::InputFloat("Half Segmentation", &lights[selectNum].HalfSegmentation);
			ImGui::InputFloat("Length", &lights[selectNum].Length);
		}

		ImGui::End();
	}

}

bool CapsuleLight::LightUse(UINT num, CapsuleLightSave& data)
{
	if (num >= lights.size()) return false;

	data.Position = lights[num].Position;
	data.Direction = lights[num].Direction;
	data.Color = lights[num].Color;

	data.Range = lights[num].Range;
	data.RangeRcp = lights[num].RangeRcp;
	data.Length = lights[num].Length;
	data.HalfSegmentation = lights[num].HalfSegmentation;

	return true;
}

void CapsuleLight::LightSelect(Objects::Ray* ray)
{
	for (size_t i = 0; i < lights.size(); i++)
	{
		box->Update(lights[i].Position);

		if (box->Intersects(ray) == true)
		{
			selectNum = i;
			lightSelect = true;

			return;
		}
	}
}
