#include "stdafx.h"
#include "Program.h"

#include "./Viewer/FreeCam.h"
#include "./Viewer/ThirdPerson.h"

#include "./Bounding/BoundingBox.h"
#include "./Bounding/ObjectsRay.h"

#include "./Executes/ExportMesh.h"
#include "./Executes/ExportAnimation.h"
#include "./Executes/DrawModel.h"
#include "./Executes/DrawTerrain.h"
#include "./Executes/ExeGui.h"
#include "./Executes/ToonShading.h"

Program::Program()
{
	States::Create();

	D3DDesc desc;
	D3D::GetDesc(&desc);

	values = new ExecuteValues();
	values->ViewProjection = new ViewProjectionBuffer();
	values->Perspective = new Perspective(desc.Width, desc.Height);
	values->Viewport = new Viewport(desc.Width, desc.Height);
	values->ViewFrustum = new Objects::Frustum;
	values->GuiSettings = new GuiSettings();
	values->GlobalLight = new LightBuffer();
	values->MainCamera = new ThirdPerson();

	executes.push_back(new ToonShading(values));
	executes.push_back(new ExportMesh(values));
	executes.push_back(new ExportAnimation(values));

	//values->MainCamera = new FreeCam(100);
	//values->MainCamera->SetPosition(0.0f, 10.0f, -10.0f);
	//values->MainCamera->SetRotationDegree(0.0f, 0.0f);

	DrawModel* model = new DrawModel(values);
	
	executes.push_back(model);
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
	
	D3DXVECTOR3 up = D3DXVECTOR3(0, 1, 0);
	D3DXVECTOR3 temp = -values->GlobalLight->Data.Direction;
	//D3DXVec3Normalize(&temp, &values->GlobalLight->Data.Direction);
	//float dot = D3DXVec3Dot(&values->GlobalLight->Data.Direction, &up);
	float dot = D3DXVec3Dot(&temp, &up);
	float intensity = Math::Clamp(dot * 1.5f, 0, 1);
	
	values->GlobalLight->Data.Intensity = intensity;
	values->ViewProjection->AddTime(Time::Delta());

	for (Execute* exe : executes)
		exe->Update();
}

void Program::PreRender()
{
	SetGlobalBuffers();
	SetLightBuffer();

	for (Execute* exe : executes)
		exe->ShadowRender();

	for (Execute* exe : executes)
		exe->PreRender();
}

void Program::Render()
{
	SetGlobalBuffers();

	for (Execute* exe : executes)
		exe->LightRender();

	for (Execute* exe : executes)
		exe->SSAORender();

	for (Execute* exe : executes)
		exe->EdgeRender();
}

void Program::PostRender()
{
	D3D::Get()->SetRenderTarget();
	
	for (Execute* exe : executes)
		exe->AARender();

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

	values->ViewFrustum->ConstructFrustum(values->Perspective->GetFarZ(), projection, view);

	values->ViewProjection->SetVSBuffer(10);
	values->ViewProjection->SetDSBuffer(10);
	values->ViewProjection->SetGSBuffer(10);
	values->ViewProjection->SetPSBuffer(10);
}

void Program::SetLightBuffer()
{
	D3DXMATRIX v;
	D3DXVECTOR3 cam;
	values->MainCamera->GetPosition(&cam);

	D3DXVECTOR3 lightPos, dir;
	dir = values->GlobalLight->Data.Direction;
	D3DXVec3Normalize(&dir, &dir);
	lightPos = cam - dir * values->Perspective->GetFarZ() * 0.5f;

	D3DXMatrixLookAtLH(&v, &lightPos, &(lightPos + values->GlobalLight->Data.Direction), &D3DXVECTOR3(0, 1, 0));
	values->GlobalLight->Data.LightPosition = lightPos;
	
	values->GlobalLight->SetLightView(v);

	values->GlobalLight->SetVSBuffer(0);
	values->GlobalLight->SetGSBuffer(0);
	values->GlobalLight->SetPSBuffer(0);
}
