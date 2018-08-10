#include "stdafx.h"
#include "Fog.h"

//�Ȱ��� ����� �Ϸ��� clear �÷��� fog �÷��� �����ִ°� �±� ��

Fog::Fog()
{
	buffer = new Buffer;

	buffer->Data.Color = D3DXCOLOR(0.75f, 0.75f, 0.75f, 1.0f);
	buffer->Data.Start = 0.0f;
	buffer->Data.End = 1.0f;
}

Fog::~Fog()
{
	SAFE_DELETE(buffer);
}

void Fog::Update()
{

}

void Fog::Render()
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
