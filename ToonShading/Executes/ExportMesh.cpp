#include "stdafx.h"
#include "ExportMesh.h"

#include "../Fbx/Exporter.h"

ExportMesh::ExportMesh(ExecuteValues* values)
	: Execute(values)
{
	Fbx::Exporter* exporter = NULL;

	//exporter = new Fbx::Exporter(Assets + L"dude/dude.fbx");
	//exporter->ExportMaterial(Models + L"dude/", L"dude");
	//exporter->ExportMesh(Models + L"dude/", L"dude");
	//exporter->ExportAnimation(Models + L"dude/", L"walk");
	//SAFE_DELETE(exporter);

	//exporter = new Fbx::Exporter(Assets + L"GOBLIN@2HitComboDaggers.fbx");
	//exporter->ExportAnimation(Models + L"Goblin/", L"2Hit");
	//SAFE_DELETE(exporter);
	//
	//exporter = new Fbx::Exporter(Assets + L"GOBLIN.fbx");
	//exporter->ExportMaterial(Models + L"Goblin/", L"Goblin");
	//exporter->ExportMesh(Models + L"Goblin/", L"Goblin");
	//SAFE_DELETE(exporter);

	//exporter = new Fbx::Exporter(Assets + L"Sapphi/Sapphiart@idle.fbx");
	//exporter->ExportAnimation(Models + L"Meshes/", L"Idle");
	//SAFE_DELETE(exporter);

	//exporter = new Fbx::Exporter(Assets + L"Meshes/Capsule.fbx");
	//exporter->ExportMaterial(Models + L"Meshes/", L"Capsule");
	//exporter->ExportMesh(Models + L"Meshes/", L"Capsule");
	//SAFE_DELETE(exporter);
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Cube.fbx");
	//exporter->ExportMaterial(Models + L"Meshes/", L"Cube");
	//exporter->ExportMesh(Models + L"Meshes/", L"Cube");
	//SAFE_DELETE(exporter);
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Cylinder.fbx");
	//exporter->ExportMaterial(Models + L"Meshes/", L"Cylinder");
	//exporter->ExportMesh(Models + L"Meshes/", L"Cylinder");
	//SAFE_DELETE(exporter);
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Plane.fbx");
	//exporter->ExportMaterial(Models + L"Meshes/", L"Plane");
	//exporter->ExportMesh(Models + L"Meshes/", L"Plane");
	//SAFE_DELETE(exporter);
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Quad.fbx");
	//exporter->ExportMaterial(Models + L"Meshes/", L"Quad");
	//exporter->ExportMesh(Models + L"Meshes/", L"Quad");
	//SAFE_DELETE(exporter);
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Sphere.fbx");
	//exporter->ExportMaterial(Models + L"Meshes/", L"Sphere");
	//exporter->ExportMesh(Models + L"Meshes/", L"Sphere");
	//SAFE_DELETE(exporter);
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Cone.fbx");
	//exporter->ExportMaterial(Models + L"Meshes/", L"Cone");
	//exporter->ExportMesh(Models + L"Meshes/", L"Cone");
	//SAFE_DELETE(exporter);

	/*
	이력서에서 가장 먼저 보게 되는 경우가 많은건 맨 위의 주소, 가급적이면 서울/경기로 있어야
	보통 받아줌 , 경력도 한번 훑지만 우린 해당없는데 하나 쓸 수 있는게 입상경력, 학원에서 우수포폴이랑
	개근상 있음 개근상은 지각결석 없으면 받는것 - 포폴은 알아서 잘 해야함
	학력사항에 컴공이면 반드시 명시(난?), 희망연봉은 신입은 무조건 회사내규, 신입은 협상 없음
	보통 테이블이 있어서 그럼 , 제목할때 충성표시같은건 너무 자신감없어보임, 그냥 신입 클라이언트 지원입니다
	같이 적당하고 무난한 문구로 끊는게 좋음 , 기술만 나열하는것도 좋지 않음 중복되는 단어 나열도 별로
	팀을 생각하고 있으면 비공개로 해서 ~~팀 지원하는 프로그래머입니다 같은 식으로 직접 보내는게 좋음

	자기소개서에선 부정적 어휘는 쓰지 않아야 함, (저는 비록 ~~하지만 ~~하려고 노력했습니다) 같은 거
	기본적, 기초적도 안좋은 어휘 , 자기소개서 읽는 팀장들은 심플한거 좋아하니까 반복은 금지, 
	기승전결로 쓸땐 앞에서 흥미를 끌고 계속 이어가던가 해야하는데 능력 안되면 결기승전으로 위쪽에 하고싶은
	말을 쓰고 아래로 쓰는 게 낫다 , 또 자기 입장말고 읽는 사람 입장으로 쓸 것 , 문장을 더 낫게
	할 방법을 잘 모르는 경우가 많음, 소리내서 읽었을때 듣기 편한게 좋은 문장 - 녹음해서 들어보면
	대충 알 수 있을 것 , 문장은 가급적 3~4줄로, 더 길면 답답해보이고 2줄이면 너무 작아보임 , ~~하게
	되었습니다도 피동적이라 좋지 않은 표현임, 자신의 선택으로 무언가를 했다는 쪽으로 얘기를 진행해야 함 ,
	단점같은거 적어두면(한개정도) 단점은 잘 안묻음, + 혹시 모르니까 한두개정도 더 생각해두면 좋음
	키워드 이용해서 위에 박아놓고 쓰면 글 쓰기 좀 더 편해짐
	경험을 쓸 게 없으면 프로그래머로서 개발할때 가장 즐거웠던 때나 힘들었던 때에 극복방법등의 경험등도
	좋다
	이력서 끝날땐 끝인사도 넣어주자, 오탈자는 당연히 없어야 함 ( 워드에서 빨간 줄 없으면 오탈자 없음)
	영어 스펠링도 너무 남용하면 별로 좋지 않음
	*/
}

ExportMesh::~ExportMesh()
{

}

void ExportMesh::Update()
{

}

void ExportMesh::ShadowRender()
{
}

void ExportMesh::PreRender()
{
}

void ExportMesh::LightRender()
{
}

void ExportMesh::SSAORender()
{
}

void ExportMesh::EdgeRender()
{
}

void ExportMesh::AARender()
{
}

void ExportMesh::ImGuiRender()
{
}
