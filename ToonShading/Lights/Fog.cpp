#include "stdafx.h"
#include "Fog.h"

//안개를 제대로 하려면 clear 컬러랑 fog 컬러를 맞춰주는게 맞긴 함

Fog::Fog()
{
	buffer = new Buffer;

	buffer->Data.Color = D3DXCOLOR(0.75f, 0.75f, 0.75f, 1.0f);
}

Fog::~Fog()
{
	SAFE_DELETE(buffer);
}

void Fog::Update()
{

}

void Fog::PreRender()
{
	buffer->SetPSBuffer(13);
}

void Fog::ImGuiRender()
{
	ImGui::Begin("Fog");

	{
		ImGui::ColorEdit3("Color", buffer->Data.Color);
		ImGui::SliderFloat("Start", &buffer->Data.Start, 0, 1);
		ImGui::SliderFloat("End", &buffer->Data.End, 0, 1);
	}

	ImGui::End();

}

void Fog::SetUse(bool val)
{
	buffer->Data.Use = (UINT)val;
}

FogSave Fog::GetFogData()
{
	FogSave temp;
	temp.Color = buffer->Data.Color;
	temp.Start = buffer->Data.Start;
	temp.End = buffer->Data.End;
	temp.Use = buffer->Data.Use;

	return temp;
}

void Fog::SetFogData(FogSave fog)
{
	buffer->Data.Color = fog.Color;
	buffer->Data.Start = fog.Start;
	buffer->Data.End = fog.End;
	buffer->Data.Use = fog.Use;
}
