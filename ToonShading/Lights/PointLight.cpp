#include "stdafx.h"
#include "PointLight.h"

#include "../Bounding/BoundingBox.h"
#include "../Bounding/ObjectsRay.h"

#include "../Objects/MeshSphere.h"

PointLight::PointLight(ExecuteValues* values)
	: values(values)
{
	buffer = new Buffer();

	D3DXVECTOR3 max, min;
	max = D3DXVECTOR3(0.5, 0.5, 0.5);
	min = -max;

	box = new Objects::BoundingBox(max, min);
	sphere = new MeshSphere();
	sphere->SetShader(Shaders + L"999_PointLightMesh.hlsl");
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
	SAFE_DELETE(psShader);
	SAFE_DELETE(meshBuffer);
	SAFE_DELETE(sphere);
	SAFE_DELETE(box);
	SAFE_DELETE(buffer);
}

UINT PointLight::AddPointLight(D3DXVECTOR3 position, D3DXVECTOR3 color, float intensity, float range)
{
	PointLightSave temp;
	temp.Position = position;
	temp.Color = color;
	temp.intenstiy = intensity;
	temp.range = range;
	lights.push_back(temp);

	return lights.size() - 1;
	//for (size_t i = 0; i < 16; i++)
	//{
	//	if (buffer->Data.Light[i].Use == 0)
	//	{
	//		buffer->Data.Light[i].Use = 1;
	//		buffer->Data.Light[i].Position = position;
	//		buffer->Data.Light[i].Color = color;
	//		buffer->Data.Light[i].Intensity = intensity;
	//		buffer->Data.Light[i].Range = range;
	//
	//		buffer->Data.Count++;
	//
	//		return i;
	//	}
	//}
	//
	//return POINTLIGHTSIZE;
}

UINT PointLight::PointLightSize()
{
	return lights.size();
	//return buffer->Data.Count;
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

		//for (size_t i = 0; i < POINTLIGHTSIZE; i++)
		//{
		//	if (buffer->Data.Light[i].Use == false) continue;
		//
		//	box->Update(buffer->Data.Light[i].Position);
		//	box->SetColor(D3DXCOLOR(buffer->Data.Light[i].Color.x, buffer->Data.Light[i].Color.y, buffer->Data.Light[i].Color.z, 1.0f));
		//	box->Render();
		//}
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
	//for (size_t i = 0; i < POINTLIGHTSIZE; i++)
	//{
	//	if (buffer->Data.Light[i].Use == false) continue;
	//
	//	sphere->SetPosition(buffer->Data.Light[i].Position);
	//	sphere->Scale(buffer->Data.Light[i].Range * 2);
	//	meshBuffer->Data.Number = i;
	//	meshBuffer->SetPSBuffer(2);
	//	sphere->PreRender();
	//}
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

				ImGui::End();
			}

			ImGui::InputFloat3("Position", lights[selectNum].Position);
			ImGui::ColorEdit3("Color", lights[selectNum].Color);
			ImGui::InputFloat("Intensity", &lights[selectNum].intenstiy);
			ImGui::InputFloat("Range", &lights[selectNum].range);
			//ImGui::Text("Count : %d", buffer->Data.Count);
			//UINT b = buffer->Data.Light[selectNum].Use;
			//ImGui::Checkbox("Use", (bool*)&buffer->Data.Light[selectNum].Use);
			//if (b != buffer->Data.Light[selectNum].Use)
			//{
			//	if (b == 1)
			//		buffer->Data.Count--;
			//	else
			//		buffer->Data.Count++;
			//}
			//
			//ImGui::InputFloat3("Position", buffer->Data.Light[selectNum].Position);
			//ImGui::ColorEdit3("Color", buffer->Data.Light[selectNum].Color);
			//ImGui::InputFloat("Intensity", &buffer->Data.Light[selectNum].Intensity);
			//ImGui::InputFloat("Range", &buffer->Data.Light[selectNum].Range);
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
	data.range = lights[num].intenstiy;

	return true;

	//if (buffer->Data.Light[num].Use == false)
	//	return false;
	//
	//data.Color = buffer->Data.Light[num].Color;
	//data.Position = buffer->Data.Light[num].Position;
	//data.intenstiy = buffer->Data.Light[num].Intensity;
	//data.range = buffer->Data.Light[num].Range;
	//
	//return buffer->Data.Light[num].Use == 1;
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

	//for (size_t i = 0; i < POINTLIGHTSIZE; i++)
	//{
	//	box->Update(buffer->Data.Light[i].Position);
	//
	//	if (box->Intersects(ray) == true)
	//	{
	//		selectNum = i;
	//		lightSelect = true;
	//
	//		return;
	//	}
	//}

	//selectNum = POINTLIGHTSIZE;
	//lightSelect = false;
}
