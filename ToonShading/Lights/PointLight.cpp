#include "stdafx.h"
#include "PointLight.h"

PointLight::PointLight()
{
	buffer = new Buffer();
}

PointLight::~PointLight()
{
	SAFE_DELETE(buffer);
}

bool PointLight::AddPointLight(D3DXVECTOR3 position, D3DXVECTOR3 color, float intensity, float range)
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

			return true;
		}
	}

	return false;
}

void PointLight::Render()
{
	buffer->SetPSBuffer(10);
}

void PointLight::ImGuiRender()
{
	for (size_t i = 0; i < 16; i++)
	{
		if (buffer->Data.Light[i].Use == 1)
		{
			ImGui::Begin("Light");

			{
				ImGui::InputFloat3("Position", buffer->Data.Light[i].Position);
				ImGui::ColorEdit3("Color", buffer->Data.Light[i].Color);
				ImGui::InputFloat("Intensity", &buffer->Data.Light[i].Intensity);
				ImGui::InputFloat("Range", &buffer->Data.Light[i].Range);
			}

			ImGui::End();
			break;
		}
	}

}
