#include "stdafx.h"
#include "ExportMesh.h"

#include "../Fbx/Exporter.h"

ExportMesh::ExportMesh(ExecuteValues* values)
	: Execute(values)
{
	Fbx::Exporter* exporter = NULL;

	//exporter = new Fbx::Exporter(Assets + L"Sapphi/Sapphiart_model.fbx");
	//exporter->ExportMaterial(Models + L"Sapphi/", L"Sapphi");
	//exporter->ExportMesh(Models + L"Sapphi/", L"Sapphi");
	//SAFE_DELETE(exporter);
	//
	//exporter = new Fbx::Exporter(Assets + L"Sapphi/Sapphiart@idle.fbx");
	//exporter->ExportAnimation(Models + L"Sapphi/", L"Idle");
	//SAFE_DELETE(exporter);
	//exporter = new Fbx::Exporter(Assets + L"Bonney/jewelry A.fbx");
	//exporter->ExportMaterial(Models + L"Bonney/", L"Bonney");
	//exporter->ExportMesh(Models + L"Bonney/", L"Bonney");
	//exporter->ExportAnimation(Models + L"Bonney/", L"Bonney");
	//SAFE_DELETE(exporter);
	//
	//exporter = new Fbx::Exporter(Assets + L"Galko/GalkoPOSE.fbx");
	//
	//exporter->ExportAnimation(Models + L"Galko/", L"GalkoPOSE");
	//SAFE_DELETE(exporter);

	////sky
	//exporter = new Fbx::Exporter(Assets + L"World/Stage1/France_Sky.fbx");
	//exporter->ExportMaterial(Models + L"Stage/", L"france_sky");
	//exporter->ExportMesh(Models + L"Stage/", L"france_sky");
	//SAFE_DELETE(exporter);

	//stage
	/*exporter = new Fbx::Exporter(Assets + L"World/Stage1/France.fbx");
	exporter->ExportMaterial(Models + L"Stage/", L"france");
	exporter->ExportMesh(Models + L"Stage/", L"france");
	SAFE_DELETE(exporter);*/
	
	//grund
	//exporter = new Fbx::Exporter(Assets + L"Grund/Grund.fbx", true);
	//exporter->ExportMaterial(Models + L"Grund/", L"grund");
	//exporter->ExportMesh(Models + L"Grund/", L"grund");
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

void ExportMesh::PreRender()
{

}

void ExportMesh::Render()
{

}

void ExportMesh::PostRender()
{

}

void ExportMesh::ImGuiRender()
{
}
