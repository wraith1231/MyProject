#pragma once

class Model;

namespace Fbx
{
	struct FbxJoint;
	struct FbxBoneData;

	struct FbxMaterial;
	struct FbxMeshData;

	struct FbxAnimation;

	class Exporter
	{
	public:
		Exporter(wstring fbxFile, bool bXna = false);
		~Exporter();

	public:
		// 수업
		void ExportMaterial(wstring saveFolder, wstring saveName);
		void ExportMesh(wstring saveFolder, wstring saveName);
		void ExportAnimation(wstring saveFolder, wstring saveName);
		void ExportAnimation(wstring saveFolder, wstring saveName, bool xmlstyle);

	private:
		void CopyTextureFile(string& textureFile, wstring saveFolder);

		void ReadBoneData(FbxNode* node, int index, int parent);
		void ReadMeshData(FbxNode* node, int parentBone);
		void ReadSkinData();
		
		void WriteMeshData(wstring saveFolder, wstring saveName);

		UINT GetBoneIndexByName(string name);

		void ReadAnimation();
		
		void ReadAnimation(
			FbxAnimation* animation, FbxNode* node,
			int start, int end
		);
		
		void WriteAnimaiton(wstring saveFolder, wstring saveName);
		void WriteAnimationXml(wstring saveFolder, wstring saveName);

		//수업
	private:
		bool bXna;	//임시

		Model* model;

		FbxManager* manager;
		FbxImporter* importer;
		FbxScene* scene;
		FbxGeometryConverter* converter;
		FbxIOSettings* ios;

	private:
		vector<struct FbxBoneData*> boneDatas;

		vector<struct FbxMaterial*> materials;
		vector<struct FbxMeshData*> meshDatas;

		vector<struct FbxAnimation*> animDatas;

	};
}