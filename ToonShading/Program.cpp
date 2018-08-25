#include "stdafx.h"
#include "Program.h"

#include "./Viewer/FreeCam.h"
#include "./Viewer/ThirdPerson.h"

#include "./Bounding/BoundingBox.h"

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
	//values->Perspective = new Perspective(desc.Width, desc.Height);
	values->Perspective = new Perspective(desc.Width, desc.Height);
	values->Viewport = new Viewport(desc.Width, desc.Height);
	values->ViewFrustum = new Objects::Frustum;
	values->GuiSettings = new GuiSettings();
	values->GlobalLight = new LightBuffer();
	values->MainCamera = new ThirdPerson();

	//globalPers = new Perspective(desc.Width, desc.Height, (float)D3DX_PI / 2, 0.1f, 1000.0f);
	//D3DXMATRIX p;
	//globalPers->GetMatrix(&p);
	//D3DXMatrixTranspose(&values->GlobalLight->Data.LightProjection, &p);

	executes.push_back(new ToonShading(values));
	executes.push_back(new ExportMesh(values));
	executes.push_back(new ExportAnimation(values));

	//values->jsonRoot = new Json::Value();
	//Json::ReadData(values->jsonRoot);

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

	//SAFE_DELETE(globalPers);
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

	for (Execute* exe : executes)
		exe->Update();
}

void Program::PreRender()
{
	SetGlobalBuffers();

	for (Execute* exe : executes)
		exe->ShadowRender();

	for (Execute* exe : executes)
		exe->PreRender();

}

void Program::Render()
{
	for (Execute* exe : executes)
		exe->LightRender();

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

	//values->ViewFrustum->SetMatrix(view * projection);
	values->ViewFrustum->ConstructFrustum(values->Perspective->GetFarZ(), projection, view);

	values->ViewProjection->SetVSBuffer(10);
	values->ViewProjection->SetDSBuffer(10);
	values->ViewProjection->SetPSBuffer(10);

	//D3DXMATRIX v, p;
	//D3DXMatrixIdentity(&v);
	//D3DXMatrixIdentity(&p);
	//
	////view
	//vector<D3DXVECTOR3> corner;
	////D3DXVECTOR3 corner[8];
	//for (UINT i = 0; i < 8; i++)
	//	corner.push_back(values->ViewFrustum->GetCorner(i));
	//Objects::BoundingBox* box = NULL;
	//Objects::BoundingBox::CreateFromPoints(corner, &box);
	//
	//D3DXVECTOR3 boxMax = box->GetMax(), boxMin = box->GetMin();
	//SAFE_DELETE(box);
	//
	//D3DXVECTOR3 boxSize = boxMax - boxMin;
	//D3DXVECTOR3 centroid = boxMin + boxSize * 0.5f;
	//
	//float dist = D3DXVec3Length(&D3DXVECTOR3(centroid - boxMin));
	//D3DXVECTOR3 dir;
	//D3DXVec3Normalize(&dir, &values->GlobalLight->Data.Direction);
	//D3DXVECTOR3 lightPos = centroid + (dir * dist);
	//v = Math::CreateLookAt(lightPos, centroid, D3DXVECTOR3(0, 1, 0));
	//
	////projection
	//for (UINT i = 0; i < 8; i++)
	//	corner[i] = Math::Transform(corner[i], v);
	//	//D3DXVec3TransformCoord(&corner[i], &corner[i], &v);
	//D3DXVECTOR3 max, min;
	//max = D3DXVECTOR3(-999999, -999999, -999999);
	//min = -max;
	//for (UINT i = 0; i < 8; i++)
	//{
	//	max = Math::Max(max, corner[i]);
	//	min = Math::Min(min, corner[i]);
	//}
	//
	////float clipDist = fabs(max.z - min.z);
	//
	////left = min.x, right = max.x, bottom = min.y, top = max.y, zn = 0.0f, zf = clipDist
	//p = Math::CreateOrthographicOffCenter(min.x, max.x, min.y, max.y, -max.z, -min.z);
	//
	////values->GlobalLight->Data.LightView = v;
	////values->GlobalLight->Data.LightProjection = p;
	//
	//D3DXMatrixTranspose(&values->GlobalLight->Data.LightView, &v);
	//D3DXMatrixTranspose(&values->GlobalLight->Data.LightProjection, &p);

	values->GlobalLight->SetVSBuffer(0);
	values->GlobalLight->SetPSBuffer(0);
}
