#include "stdafx.h"
#include "PointLight.h"

#include "../Bounding/BoundingBox.h"
#include "../Bounding/ObjectsRay.h"
#include "../Bounding/BoundingSphere.h"

#include "../Objects/MeshSphere.h"

PointLight::PointLight(ExecuteValues* values)
	: values(values)
{
	buffer = new Buffer();

	D3DXVECTOR3 max, min;
	max = D3DXVECTOR3(0.5, 0.5, 0.5);
	min = -max;

	box = new Objects::BoundingBox(max, min);

	meshBuffer = new MeshBuffer();

	psShader = new Shader(Shaders + L"PointLight.hlsl");
	psShader->CreateHullShader(Shaders + L"PointLight.hlsl");
	psShader->CreateDomainShader(Shaders + L"PointLight.hlsl");

	{
		D3D11_BLEND_DESC desc;
		//States::GetBlendDesc(&desc);
		//States::CreateBlend(&desc, &blend[0]);

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

PointLight::~PointLight()
{
	SAFE_RELEASE(rasterize);
	SAFE_RELEASE(blend);

	SAFE_DELETE(dsBuffer);
	SAFE_DELETE(meshBuffer);
	SAFE_DELETE(buffer);

	SAFE_DELETE(psShader);

	SAFE_DELETE(box);
}

UINT PointLight::AddPointLight(D3DXVECTOR3 position, D3DXVECTOR3 color, float intensity, float range, bool add)
{
	PointLightSave temp;
	temp.Position = position;
	if(add == true)
		temp.Position.y += 5.0f;
	temp.Color = color;
	temp.intenstiy = intensity;
	temp.range = range;
	lights.push_back(temp);

	return lights.size() - 1;
}

UINT PointLight::PointLightSize()
{
	return lights.size();
}

void PointLight::PreRender()
{
	buffer->SetPSBuffer(10);

	if (lightSelect == true)
	{
		box->Update(lights[selectNum].Position);
		box->SetColor(D3DXCOLOR(lights[selectNum].Color.x, lights[selectNum].Color.y, lights[selectNum].Color.z, 1.0f));
		box->Render();
	}
}

void PointLight::PreRender(bool val)
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

void PointLight::LightRender()
{
	ID3D11BlendState* prevBlen;
	FLOAT prevFactor[4];
	UINT prevMask;
	D3D::GetDC()->OMGetBlendState(&prevBlen, prevFactor, &prevMask);
	D3D::GetDC()->OMSetBlendState(blend, prevFactor, prevMask);

	ID3D11RasterizerState* prevRaster;
	D3D::GetDC()->RSGetState(&prevRaster);
	D3D::GetDC()->RSSetState(rasterize);
	values->ViewProjection->GetProjection(dsBuffer->Data.mat);
	D3DXMatrixTranspose(&dsBuffer->Data.mat, &dsBuffer->Data.mat);
	dsBuffer->SetDSBuffer(1);

	for (PointLightSave light : lights)
	{
		if (values->ViewFrustum->CheckSphere(light.Position.x, light.Position.y, light.Position.z, light.range) == false) continue;

		buffer->Data.Position = light.Position;
		buffer->Data.Color = light.Color;
		buffer->Data.RangeRcp = 1.0f / light.range;

		D3D::GetDC()->IASetInputLayout(NULL);
		D3D::GetDC()->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
		D3D::GetDC()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);

		buffer->SetPSBuffer(2);
		psShader->Render();
		D3D::GetDC()->Draw(2, 0);
	}

	D3D::GetDC()->RSSetState(prevRaster);
	D3D::GetDC()->OMSetBlendState(prevBlen, prevFactor, prevMask);

	Shader::ClearShader();
}

void PointLight::ImGuiRender()
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
			ImGui::InputFloat("Intensity", &lights[selectNum].intenstiy);
			ImGui::InputFloat("Range", &lights[selectNum].range);
		}

		ImGui::End();
	}

}

bool PointLight::LightUse(UINT num, PointLightSave& data)
{
	if (num >= lights.size()) return false;

	data.Color = lights[num].Color;
	data.Position = lights[num].Position;
	data.intenstiy = lights[num].intenstiy;
	data.range = lights[num].range;

	return true;
}

void PointLight::LightSelect(Objects::Ray* ray)
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
