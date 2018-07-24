#pragma once
#include "stdafx.h"

namespace Fbx
{
	struct FbxMaterial
	{
		string Name;	//엄청 중요함, 
						//Model Mesh는 그리기위한 단위, 사람의 경우 보통 Mesh가 하나나오지만 탱크같은건 12개가 나옴
						//그래서 material별로 분리를 하는데 그 결과물이 meshpart
						//애니메이션 가기 전에 vertex버퍼를 meshpart에서 mesh로 올릴것
						//part는 각 material 정보, mesh는 자기 mesh가 그려질 world를 가지고 세팅하게 됨
						//버퍼를 파트마다 주게 되면 엄청 느려짐 - 가장 비용이 큰게 토플로즈(트라이앵글같은거)
						//그리고 쉐이더 교체, 마지막으로 버퍼교체임 - 만약 버텍스버퍼 주소가 같다면 교체하지 않고
						//해서 속도가 훨씬 빨라짐 - 그래서 버퍼는 mesh로 통합시킬 것
						//인덱스도 일단은 버텍스하나당 +1 해줬지만 점차 중복되는 거는 그냥 인덱스로 해서 점차 줄여나갈 것
						//Material name이 중요한 이유는 불러서 그걸 part에 박아놓고
						//part가 참조하는 방식으로 할 것, part의 name과 material의 name을 매칭시켜서 불러오는 방식으로 진행
		D3DXCOLOR Ambient;
		D3DXCOLOR Diffuse;
		D3DXCOLOR Specular;			//모델 하고 나면 정반사광 바로 할 것
		D3DXCOLOR Emissive;

		float Shineness;			//specular에 들어갈 값

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