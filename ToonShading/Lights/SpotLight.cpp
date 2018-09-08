#include "stdafx.h"
#include "SpotLight.h"

#include "../Bounding/BoundingBox.h"
#include "../Bounding/ObjectsRay.h"
#include "../Bounding/BoundingSphere.h"

#include "../Objects/MeshSphere.h"

SpotLight::SpotLight(ExecuteValues* values)
	: values(values)
{
	buffer = new Buffer();

	D3DXVECTOR3 max, min;
	max = D3DXVECTOR3(0.5, 0.5, 0.5);
	min = -max;

	box = new Objects::BoundingBox(max, min);

	psShader = new Shader(Shaders + L"SpotLight.hlsl");
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

SpotLight::~SpotLight()
{
	SAFE_RELEASE(rasterize);
	SAFE_RELEASE(blend);

	SAFE_DELETE(dsBuffer);
	SAFE_DELETE(buffer);

	SAFE_DELETE(psShader);

	SAFE_DELETE(box);
}

UINT SpotLight::AddSpotLight(D3DXVECTOR3 Position, D3DXVECTOR3 Color, D3DXVECTOR3 Dir, float Range, float CosOuter, float CosInner, bool add)
{
	SpotLightSave temp;
	temp.Position = Position;
	if (add == true)
		temp.Position.y += 2.0f;
	temp.Color = Color;
	temp.Direction = Dir;

	temp.RangeRcp = Range;
	temp.CosOuter = CosOuter;
	temp.CosInner = CosInner;

	lights.push_back(temp);

	return lights.size() - 1;
}

UINT SpotLight::SpotLightSize()
{
	return lights.size();
}

void SpotLight::PreRender()
{
	buffer->SetPSBuffer(10);

	if (lightSelect == true)
	{
		box->Update(lights[selectNum].Position);
		box->SetColor(D3DXCOLOR(lights[selectNum].Color.x, lights[selectNum].Color.y, lights[selectNum].Color.z, 1.0f));
		box->Render();
	}
}

void SpotLight::PreRender(bool val)
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

void SpotLight::LightRender()
{
	ID3D11BlendState* prevBlen;
	FLOAT prevFactor[4];
	UINT prevMask;
	D3D::GetDC()->OMGetBlendState(&prevBlen, prevFactor, &prevMask);
	D3D::GetDC()->OMSetBlendState(blend, prevFactor, prevMask);

	ID3D11RasterizerState* prevRaster;
	D3D::GetDC()->RSGetState(&prevRaster);
	D3D::GetDC()->RSSetState(rasterize);

	for (SpotLightSave light : lights)
	{
		if (values->ViewFrustum->CheckSphere(light.Position.x, light.Position.y, light.Position.z, light.RangeRcp) == false) continue;

		dsBuffer->Data.SinAngle = sinf(light.CosOuter);
		dsBuffer->Data.CosAngle = cosf(light.CosOuter);

		buffer->Data.Position = light.Position;
		buffer->Data.Direction = -light.Direction;
		buffer->Data.Color = light.Color;
		
		float inner, outer;
		inner = cosf(light.CosInner);
		outer = cosf(light.CosOuter);

		buffer->Data.CosAttnRange = inner - outer;
		buffer->Data.CosOuter = outer;
		buffer->Data.RangeRcp = 1.0f / light.RangeRcp;

		D3D::GetDC()->IASetInputLayout(NULL);
		D3D::GetDC()->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
		D3D::GetDC()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);

		buffer->SetPSBuffer(2);
		dsBuffer->SetDSBuffer(2);
		psShader->Render();
		D3D::GetDC()->Draw(1, 0);
	}

	D3D::GetDC()->RSSetState(prevRaster);
	D3D::GetDC()->OMSetBlendState(prevBlen, prevFactor, prevMask);

	Shader::ClearShader();
}

void SpotLight::ImGuiRender()
{
	if (lightSelect == true)
	{
		ImGui::Begin("Point Light");

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

			ImGui::InputFloat("Range", &lights[selectNum].RangeRcp);
			ImGui::SliderFloat("Cos Inner", &lights[selectNum].CosInner, 0.0f, 1.0f);
			ImGui::SliderFloat("Cos Outer", &lights[selectNum].CosOuter, 0.0f, 1.0f);
		}

		ImGui::End();
	}

}

bool SpotLight::LightUse(UINT num, SpotLightSave& data)
{
	if (num >= lights.size()) return false;

	data.Position = lights[num].Position;
	data.Direction = lights[num].Direction;
	data.Color = lights[num].Color;

	data.RangeRcp = lights[num].RangeRcp;
	data.CosInner = lights[num].CosInner;
	data.CosOuter = lights[num].CosOuter;

	return true;
}

void SpotLight::LightSelect(Objects::Ray* ray)
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
