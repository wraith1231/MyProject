#include "stdafx.h"
#include "ExeGui.h"

ExeGui::ExeGui(ExecuteValues* values)
	: Execute(values)
{

}

ExeGui::~ExeGui()
{
	
}

void ExeGui::Update()
{
	if (Keyboard::Get()->Press(VK_CONTROL))
	{
		if (Keyboard::Get()->Down(VK_F1))
			ChangeShowEnvironmentWindow();

		if (Keyboard::Get()->Down(VK_F2))
			ChangeShowLandscapeWindow();

		if (Keyboard::Get()->Down(VK_F10))
			ChangeShowGlobalLightWindow();

		if (Keyboard::Get()->Down(VK_F11))
			ChangeShowSystemInfoWindow();

		if (Keyboard::Get()->Down(VK_F12))
			ChangeShowDemoWindow();
	}
}

void ExeGui::DepthRender()
{
}

void ExeGui::NormalRender()
{
}

void ExeGui::DiffuseRender()
{
}

void ExeGui::LightRender()
{
}

void ExeGui::EdgeRender()
{
}

void ExeGui::AARender()
{
}

void ExeGui::ImGuiRender()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Environment", "Ctrl+F1"))
				ChangeShowEnvironmentWindow();

			if (ImGui::MenuItem("Landscape", "Ctrl+F2"))
				ChangeShowLandscapeWindow();

			if (ImGui::MenuItem("GlobalLight", "Ctrl+F10"))
				ChangeShowGlobalLightWindow();

			if (ImGui::MenuItem("SystemInfo", "Ctrl+F11"))
				ChangeShowSystemInfoWindow();

			if (ImGui::MenuItem("Demo", "Ctrl+F12"))
				ChangeShowDemoWindow();

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}


	D3DXVECTOR3 cameraPosition;
	D3DXVECTOR2 cameraRotation;
	values->MainCamera->GetPosition(&cameraPosition);
	values->MainCamera->GetRotationDegree(&cameraRotation);

	if (values->GuiSettings->bShowSystemInfoWindow == true)
	{
		ImGui::Begin("System Info", &values->GuiSettings->bShowSystemInfoWindow);
		ImGui::Text("Frame Per Second : %4.0f", ImGui::GetIO().Framerate);
		ImGui::Separator();
		ImGui::Text
		(
			"Camera Position : %3.0f, %3.0f, %3.0f"
			, cameraPosition.x, cameraPosition.y, cameraPosition.z
		);
		ImGui::Text
		(
			"Camera Rotation : %3.0f, %3.0f", cameraRotation.x, cameraRotation.y
		);

		ImGui::Separator();

		ImGui::SliderFloat3("Global Light", values->GlobalLight->Data.Direction, -1, 1);

		ImGui::End();
	}
	if (values->GuiSettings->bShowGlobalLightWindow == true)
	{
		ImGui::Begin("Global Light", &values->GuiSettings->bShowGlobalLightWindow);


		ImGui::SliderFloat3("Direction", values->GlobalLight->Data.Direction, -1, 1);
		ImGui::ColorEdit3("Color", values->GlobalLight->Data.Color);

		ImGui::Separator();

		ImGui::SliderFloat("Intensity", &values->GlobalLight->Data.Intensity, -1, 1);
		ImGui::SliderFloat("Overcast", &values->GlobalLight->Data.Overcast, 0, 1);

		ImGui::End();
	}

	if (values->GuiSettings->bShowDemoWindow == true)
	{
		ImGui::ShowDemoWindow(&values->GuiSettings->bShowDemoWindow);
	}
}

void ExeGui::ChangeShowEnvironmentWindow()
{
	values->GuiSettings->bShowEnvironmentWindow =
		!values->GuiSettings->bShowEnvironmentWindow;
}

void ExeGui::ChangeShowLandscapeWindow()
{
	values->GuiSettings->bShowLandscapeWindow =
		!values->GuiSettings->bShowLandscapeWindow;
}

void ExeGui::ChangeShowSystemInfoWindow()
{
	values->GuiSettings->bShowSystemInfoWindow =
		!values->GuiSettings->bShowSystemInfoWindow;
}

void ExeGui::ChangeShowDemoWindow()
{
	values->GuiSettings->bShowDemoWindow =
		!values->GuiSettings->bShowDemoWindow;
}


void ExeGui::ChangeShowGlobalLightWindow()
{
	values->GuiSettings->bShowGlobalLightWindow =
		!values->GuiSettings->bShowGlobalLightWindow;
}
