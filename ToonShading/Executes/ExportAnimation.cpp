#include "stdafx.h"
#include "ExportAnimation.h"

#include "../Fbx/XmlToAnim.h"

ExportAnimation::ExportAnimation(ExecuteValues* values)
	: Execute(values)
{
	Fbx::XmlToAnim* anim = NULL;


	//Player
	//vector<wstring> files;
	//Path::GetFiles(files, Assets + L"RobotAnim/Players/Grund/", L"*.Animation", false);
	//
	//for (wstring file : files)
	//{
	//	wstring name = Path::GetFileNameWithoutExtension(file);
	//
	//	anim = new Fbx::XmlToAnim(file);
	//	anim->ExportAnimation(Models + L"Grund/", name);
	//	SAFE_DELETE(anim);
	//}
	//
	//int a = 10;
}

ExportAnimation::~ExportAnimation()
{

}

void ExportAnimation::Update()
{

}

void ExportAnimation::ShadowRender()
{
}

void ExportAnimation::PreRender()
{
}

void ExportAnimation::LightRender()
{
}

void ExportAnimation::EdgeRender()
{
}

void ExportAnimation::AARender()
{
}

void ExportAnimation::ImGuiRender()
{
}
