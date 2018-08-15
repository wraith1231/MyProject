#include "stdafx.h"
#include "PointLight.h"

#include "../Bounding/BoundingBox.h"
#include "../Bounding/ObjectsRay.h"

#include "../Objects/MeshSphere.h"

PointLight::PointLight()
{
	buffer = new Buffer();

	D3DXVECTOR3 max, min;
	max = D3DXVECTOR3(0.5, 0.5, 0.5);
	min = -max;

	box = new Objects::BoundingBox(max, min);
	sphere = new MeshSphere();
	sphere->SetShader(Shaders + L"999_PointLightMesh.hlsl");
	meshBuffer = new MeshBuffer();
}

PointLight::~PointLight()
{
	SAFE_DELETE(meshBuffer);
	SAFE_DELETE(sphere);
	SAFE_DELETE(box);
	SAFE_DELETE(buffer);
}

UINT PointLight::AddPointLight(D3DXVECTOR3 position, D3DXVECTOR3 color, float intensity, float range)
{
	for (size_t i = 0; i < 16; i++)
	{
		if (buffer->Data.Light[i].Use == 0)
		{
			buffer->Data.Light[i].Use = 1;
			buffer->Data.Light[i].Position = position;
			buffer->Data.Light[i].Color = color;
			buffer->Data.Light[i].Intensity = intensity;
			buffer->Data.Light[i].Range = range;

			buffer->Data.Count++;

			return i;
		}
	}

	return POINTLIGHTSIZE;
}

UINT PointLight::PointLightSize()
{
	return buffer->Data.Count;
}

void PointLight::PreRender()
{
	buffer->SetPSBuffer(10);

	if (lightSelect == true)
	{
		box->Update(buffer->Data.Light[selectNum].Position);
		box->SetColor(D3DXCOLOR(buffer->Data.Light[selectNum].Color.x, buffer->Data.Light[selectNum].Color.y, buffer->Data.Light[selectNum].Color.z, 1.0f));
		box->Render();
	}
}

void PointLight::PreRender(bool val)
{
	buffer->SetPSBuffer(10);

	if (val == true)
	{
		for (size_t i = 0; i < POINTLIGHTSIZE; i++)
		{
			if (buffer->Data.Light[i].Use == false) continue;

			box->Update(buffer->Data.Light[i].Position);
			box->SetColor(D3DXCOLOR(buffer->Data.Light[i].Color.x, buffer->Data.Light[i].Color.y, buffer->Data.Light[i].Color.z, 1.0f));
			box->Render();
		}
	}
}

void PointLight::LightMeshRender()
{
	for (size_t i = 0; i < POINTLIGHTSIZE; i++)
	{
		if (buffer->Data.Light[i].Use == false) continue;

		sphere->SetPosition(buffer->Data.Light[i].Position);
		sphere->Scale(buffer->Data.Light[i].Range * 2);
		meshBuffer->Data.Number = i;
		meshBuffer->SetPSBuffer(2);
		sphere->PreRender();
	}
}

void PointLight::ImGuiRender()
{
	if (lightSelect == true)
	{
		ImGui::Begin("Point Light");

		{
			ImGui::Text("Count : %d", buffer->Data.Count);
			UINT b = buffer->Data.Light[selectNum].Use;
			ImGui::Checkbox("Use", (bool*)&buffer->Data.Light[selectNum].Use);
			if (b != buffer->Data.Light[selectNum].Use)
			{
				if (b == 1)
					buffer->Data.Count--;
				else
					buffer->Data.Count++;
			}

			ImGui::InputFloat3("Position", buffer->Data.Light[selectNum].Position);
			ImGui::ColorEdit3("Color", buffer->Data.Light[selectNum].Color);
			ImGui::InputFloat("Intensity", &buffer->Data.Light[selectNum].Intensity);
			ImGui::InputFloat("Range", &buffer->Data.Light[selectNum].Range);
		}

		ImGui::End();
	}

}

bool PointLight::LightUse(UINT num, PointLightSave& data)
{
	if (buffer->Data.Light[num].Use == false)
		return false;

	data.Color = buffer->Data.Light[num].Color;
	data.Position = buffer->Data.Light[num].Position;
	data.intenstiy = buffer->Data.Light[num].Intensity;
	data.range = buffer->Data.Light[num].Range;

	return buffer->Data.Light[num].Use == 1;
}

void PointLight::LightSelect(Objects::Ray* ray)
{
	for (size_t i = 0; i < POINTLIGHTSIZE; i++)
	{
		box->Update(buffer->Data.Light[i].Position);

		if (box->Intersects(ray) == true)
		{
			selectNum = i;
			lightSelect = true;

			return;
		}
	}

	//selectNum = POINTLIGHTSIZE;
	//lightSelect = false;
}
