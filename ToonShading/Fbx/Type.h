#pragma once
#include "stdafx.h"

namespace Fbx
{
	struct FbxMaterial
	{
		string Name;	//��û �߿���, 
						//Model Mesh�� �׸������� ����, ����� ��� ���� Mesh�� �ϳ��������� ��ũ������ 12���� ����
						//�׷��� material���� �и��� �ϴµ� �� ������� meshpart
						//�ִϸ��̼� ���� ���� vertex���۸� meshpart���� mesh�� �ø���
						//part�� �� material ����, mesh�� �ڱ� mesh�� �׷��� world�� ������ �����ϰ� ��
						//���۸� ��Ʈ���� �ְ� �Ǹ� ��û ������ - ���� ����� ū�� ���÷���(Ʈ���̾ޱ۰�����)
						//�׸��� ���̴� ��ü, ���������� ���۱�ü�� - ���� ���ؽ����� �ּҰ� ���ٸ� ��ü���� �ʰ�
						//�ؼ� �ӵ��� �ξ� ������ - �׷��� ���۴� mesh�� ���ս�ų ��
						//�ε����� �ϴ��� ���ؽ��ϳ��� +1 �������� ���� �ߺ��Ǵ� �Ŵ� �׳� �ε����� �ؼ� ���� �ٿ����� ��
						//Material name�� �߿��� ������ �ҷ��� �װ� part�� �ھƳ���
						//part�� �����ϴ� ������� �� ��, part�� name�� material�� name�� ��Ī���Ѽ� �ҷ����� ������� ����
		D3DXCOLOR Ambient;
		D3DXCOLOR Diffuse;
		D3DXCOLOR Specular;			//�� �ϰ� ���� ���ݻ籤 �ٷ� �� ��
		D3DXCOLOR Emissive;

		float Shineness;			//specular�� �� ��

		string DiffuseFile;
		string SpecularFile;
		string EmissiveFile;
		string NormalFile;
	};

	struct FbxBoneData
	{
		int Index;
		string Name;

		int Parent;

		D3DXMATRIX Transform;
		D3DXMATRIX AbsoluteTransform;
	};

	struct FbxVertex
	{
		string MaterialName;
		int ControlPoint;

		ModelVertexType Vertex;
	};

	struct FbxMeshPartData
	{
		string MaterialName;

		vector<ModelVertexType> Vertices;
		vector<UINT> Indices;
	};

	struct FbxMeshData
	{
		string Name;
		int ParentBone;

		FbxMesh* Mesh;

		vector<FbxVertex*> Vertices;
		vector<FbxMeshPartData *> MeshParts;
	};

	struct FbxKeyFrameData
	{
		D3DXMATRIX Transform;
		D3DXQUATERNION Rotation;
		D3DXVECTOR3 Translate;
		D3DXVECTOR3 Scale;
	};

	struct FbxKeyframe
	{
		string BoneName;
		vector<FbxKeyFrameData> Transforms;
	};

	struct FbxAnimation
	{
		string Name;	

		int TotalFrame;
		float FrameRate;

		vector<FbxKeyframe*> KeyFrames;
	};
}