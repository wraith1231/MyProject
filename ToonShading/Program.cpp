#include "stdafx.h"
#include "Program.h"

#include "./Viewer/FreeCam.h"
#include "./Viewer/ThirdPerson.h"

#include "./Executes/ExportMesh.h"
#include "./Executes/ExportAnimation.h"
#include "./Executes/DrawModel.h"
#include "./Executes/DrawTerrain.h"
#include "./Executes/ExeGui.h"
#include "./Executes/ToonShading.h"
#include "./Executes/ToonShading2.h"

Program::Program()
{
	States::Create();

	D3DDesc desc;
	D3D::GetDesc(&desc);

	values = new ExecuteValues();
	values->ViewProjection = new ViewProjectionBuffer();
	values->Perspective = new Perspective(desc.Width, desc.Height);
	values->Viewport = new Viewport(desc.Width, desc.Height);
	values->GuiSettings = new GuiSettings();
	values->GlobalLight = new LightBuffer();

	executes.push_back(new ToonShading(values));
	//executes.push_back(new ToonShading2(values));
	executes.push_back(new ExportMesh(values));
	executes.push_back(new ExportAnimation(values));

	//values->jsonRoot = new Json::Value();
	//Json::ReadData(values->jsonRoot);

	values->MainCamera = new FreeCam(100);
	values->MainCamera->SetPosition(0.0f, 0.0f, -5.0f);
	values->MainCamera->SetRotationDegree(0.0f, 0.0f);

	DrawModel* model = new DrawModel(values);
	
	//values->MainCamera = new ThirdPerson(model->GetCharPos());
	executes.push_back(model);
	//executes.push_back(terrain);
	//executes.push_back(new DrawModel(values));
	//executes.push_back(new DrawTerrain(values));
	executes.push_back(new ExeGui(values));
}

Program::~Program()
{
	for (Execute* exe : executes)
		SAFE_DELETE(exe);

	SAFE_DELETE(values->jsonRoot);
	SAFE_DELETE(values->MainCamera);
	SAFE_DELETE(values->GlobalLight);
	SAFE_DELETE(values->ViewProjection);
	SAFE_DELETE(values->Perspective);
	SAFE_DELETE(values->Viewport);
	SAFE_DELETE(values->GuiSettings);
	SAFE_DELETE(values);

	States::Delete();
}

void Program::Update()
{
	values->MainCamera->Update();

	for (Execute* exe : executes)
		exe->Update();
}

void Program::PreRender()
{
	SetGlobalBuffers();

	for (Execute* exe : executes)
		exe->PreRender();

	//SetGlobalBuffers();
	for (Execute* exe : executes)
		exe->PreRender2();

	
}

void Program::Render()
{
	for (Execute* exe : executes)
		exe->Render();

}

void Program::PostRender()
{
	for (Execute* exe : executes)
		exe->PostRender();

	D3D::Get()->SetRenderTarget();
	
	for (Execute* exe : executes)
		exe->PostRender2();

	for (Execute* exe : executes)
		exe->ImGuiRender();
}

void Program::ResizeScreen()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	values->Perspective->Set(desc.Width, desc.Height);
	values->Viewport->Set(desc.Width, desc.Height);

	for (Execute* exe : executes)
		exe->ResizeScreen();

}

void Program::SetGlobalBuffers()
{
	D3DXMATRIX view, projection;
	values->MainCamera->GetMatrix(&view);
	values->Perspective->GetMatrix(&projection);

	values->ViewProjection->SetView(view);
	values->ViewProjection->SetProjection(projection);
	values->ViewProjection->SetVSBuffer(0);

	values->GlobalLight->SetPSBuffer(0);
}
