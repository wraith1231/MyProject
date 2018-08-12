#include "stdafx.h"
#include "SpotLight.h"

#include "../Bounding/BoundingBox.h"
#include "../Bounding/ObjectsRay.h"

SpotLight::SpotLight()
{
	buffer = new Buffer();

	D3DXVECTOR3 max, min;
	max = D3DXVECTOR3(0.5, 0.5, 0.5);
	min = -max;

	box = new Objects::BoundingBox(max, min);
}

SpotLight::~SpotLight()
{
	SAFE_DELETE(box);
	SAFE_DELETE(buffer);
}

UINT SpotLight::AddSpotLight(D3DXVECTOR3 position, D3DXVECTOR3 color, D3DXVECTOR3 dir,	float inner, float outer)
{
	for (size_t i = 0; i < 32; i++)
	{
		if (buffer->Data.Light[i].Use == 0)
		{
			buffer->Data.Light[i].Use = 1;
			buffer->Data.Light[i].Position = position;
			buffer->Data.Light[i].Color = color;
			buffer->Data.Light[i].Direction = dir;
			buffer->Data.Light[i].InnerAngle = inner;
			buffer->Data.Light[i].OuterAngle = outer;

			buffer->Data.Count++;

			return i;
		}
	}

	return SPOTLIGHTSIZE;
}

UINT SpotLight::SpotLightSize()
{
	return buffer->Data.Count;
}

void SpotLight::DiffuseRender()
{
	buffer->SetPSBuffer(11);

	if (lightSelect == true)
	{
		box->Update(buffer->Data.Light[selectNum].Position);
		box->SetColor(D3DXCOLOR(buffer->Data.Light[selectNum].Color.x, buffer->Data.Light[selectNum].Color.y, buffer->Data.Light[selectNum].Color.z, 1.0f));
		box->Render();
	}
}

void SpotLight::DiffuseRender(bool val)
{
	buffer->SetPSBuffer(11);

	if (val == true)
	{
		for (size_t i = 0; i < SPOTLIGHTSIZE; i++)
		{
			if (buffer->Data.Light[i].Use == false) continue;

			box->Update(buffer->Data.Light[i].Position);
			box->SetColor(D3DXCOLOR(buffer->Data.Light[i].Color.x, buffer->Data.Light[i].Color.y, buffer->Data.Light[i].Color.z, 1.0f));
			box->Render();
		}
	}
}

void SpotLight::ImGuiRender()
{
	if (lightSelect == true)
	{
		ImGui::Begin("Spot Light");

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
			ImGui::InputFloat3("Direction", buffer->Data.Light[selectNum].Direction);
			ImGui::InputFloat("Inner Angle", &buffer->Data.Light[selectNum].InnerAngle);
			ImGui::InputFloat("Outer Angle", &buffer->Data.Light[selectNum].OuterAngle);
		}

		ImGui::End();
	}

}

bool SpotLight::LightUse(UINT num, SpotLightSave & data)
{
	if (buffer->Data.Light[num].Use == false)
		return false;

	data.Color = buffer->Data.Light[num].Color;
	data.Position = buffer->Data.Light[num].Position;
	data.Direction = buffer->Data.Light[num].Direction;
	data.InnerAngle = buffer->Data.Light[num].InnerAngle;
	data.OuterAngle = buffer->Data.Light[num].OuterAngle;

	return buffer->Data.Light[num].Use == 1;
}

void SpotLight::LightSelect(Objects::Ray* ray)
{
	for (size_t i = 0; i < SPOTLIGHTSIZE; i++)
	{
		box->Update(buffer->Data.Light[i].Position);

		if (box->Intersects(ray) == true)
		{
			selectNum = i;
			lightSelect = true;

			return;
		}
	}
}
