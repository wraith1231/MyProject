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
	�̷¼����� ���� ���� ���� �Ǵ� ��찡 ������ �� ���� �ּ�, �������̸� ����/���� �־��
	���� �޾��� , ��µ� �ѹ� ������ �츰 �ش���µ� �ϳ� �� �� �ִ°� �Ի���, �п����� ��������̶�
	���ٻ� ���� ���ٻ��� �����Ἦ ������ �޴°� - ������ �˾Ƽ� �� �ؾ���
	�з»��׿� �İ��̸� �ݵ�� ���(��?), ��������� ������ ������ ȸ�系��, ������ ���� ����
	���� ���̺��� �־ �׷� , �����Ҷ� �漺ǥ�ð����� �ʹ� �ڽŰ������, �׳� ���� Ŭ���̾�Ʈ �����Դϴ�
	���� �����ϰ� ������ ������ ���°� ���� , ����� �����ϴ°͵� ���� ���� �ߺ��Ǵ� �ܾ� ������ ����
	���� �����ϰ� ������ ������� �ؼ� ~~�� �����ϴ� ���α׷����Դϴ� ���� ������ ���� �����°� ����

	�ڱ�Ұ������� ������ ���ִ� ���� �ʾƾ� ��, (���� ��� ~~������ ~~�Ϸ��� ����߽��ϴ�) ���� ��
	�⺻��, �������� ������ ���� , �ڱ�Ұ��� �д� ������� �����Ѱ� �����ϴϱ� �ݺ��� ����, 
	�������� ���� �տ��� ��̸� ���� ��� �̾���� �ؾ��ϴµ� �ɷ� �ȵǸ� ���������� ���ʿ� �ϰ����
	���� ���� �Ʒ��� ���� �� ���� , �� �ڱ� ���帻�� �д� ��� �������� �� �� , ������ �� ����
	�� ����� �� �𸣴� ��찡 ����, �Ҹ����� �о����� ��� ���Ѱ� ���� ���� - �����ؼ� ����
	���� �� �� ���� �� , ������ ������ 3~4�ٷ�, �� ��� ����غ��̰� 2���̸� �ʹ� �۾ƺ��� , ~~�ϰ�
	�Ǿ����ϴٵ� �ǵ����̶� ���� ���� ǥ����, �ڽ��� �������� ���𰡸� �ߴٴ� ������ ��⸦ �����ؾ� �� ,
	���������� ����θ�(�Ѱ�����) ������ �� �ȹ���, + Ȥ�� �𸣴ϱ� �ѵΰ����� �� �����صθ� ����
	Ű���� �̿��ؼ� ���� �ھƳ��� ���� �� ���� �� �� ������
	������ �� �� ������ ���α׷��ӷμ� �����Ҷ� ���� ��ſ��� ���� ������� ���� �غ�������� ����
	����
	�̷¼� ������ ���λ絵 �־�����, ��Ż�ڴ� �翬�� ����� �� ( ���忡�� ���� �� ������ ��Ż�� ����)
	���� ���縵�� �ʹ� �����ϸ� ���� ���� ����
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
