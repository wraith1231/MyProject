#pragma once

class GizmoAxis;

class ModelBuffer : public ShaderBuffer
{
public:
	ModelBuffer() : ShaderBuffer(&data, sizeof(data))
	{
		for (int i = 0; i < 256; i++)
			D3DXMatrixIdentity(&data.Bones[i]);
	}

	void SetBones(D3DXMATRIX* m, UINT count)
	{
		memcpy(data.Bones, m, sizeof(D3DXMATRIX) * count);

		for (UINT i = 0; i < count; i++)
			D3DXMatrixTranspose(&data.Bones[i], &data.Bones[i]);
	}

	D3DXMATRIX GetBoneMatrix(UINT num) 
	{
		D3DXMATRIX temp = data.Bones[num];
		D3DXMatrixTranspose(&temp, &temp);
		return temp; 
	}

	void SetBoneMatrix(UINT num, D3DXMATRIX mat)
	{
		data.Bones[num] = mat;
		D3DXMatrixTranspose(&data.Bones[num], &data.Bones[num]);
	}
	//ModelAnimPlayer에서 SkinTransform을 여기다 넣어서 shader에서 rendering 들어갈 것
private:
	struct Struct
	{
		D3DXMATRIX Bones[256];
	} data;
};

class Model
{
public:
	Model();
	~Model();

	void Update();
	void PreRender();
	void PreRender2();
	void Render();

	void GizmoRender();
	void CapsuleRender();
	void PostRender();
	void ModelTree(class ModelBone* bone);

	void GizmoUpdate();
	void CapsuleWorld(D3DXMATRIX mat);
	void CapsuleScale(float scale);

	vector<class ModelMesh*>& Meshes() { return meshes; }
	vector<class ModelBone*>& Bones() { return bones; }

	void ReadMaterial(wstring file);
	void ReadMesh(wstring file);
	//void ReadAnim(wstring file);
	void ReadAnim(wstring file, wstring animName);

	Material* GetMaterial(wstring name);
	vector<Material*>& Materials() { return materials; }

	UINT MeshCount() { return meshes.size(); }
	class ModelMesh* Mesh(UINT index) { return meshes[index]; }
	class ModelMesh* Mesh(wstring name);

	UINT BoneCount() { return bones.size(); }
	class ModelBone* Bone(UINT index) { return bones[index]; }
	class ModelBone* Bone(wstring name);

	//class ModelAnimClip* Clip(UINT index) { return clips[index]; }
	vector<class ModelAnimClip*> Clips(wstring name) { return clips[name]; }

	class ModelBone* Root() { return root; }

	//Absolute가 붙으면 보통 절대좌표를 의미함
	void CopyAbsoluteBoneTo(vector<D3DXMATRIX>& transforms);
	void CopyAbsoluteBoneTo(D3DXMATRIX mat, vector<D3DXMATRIX>& transforms);

	GizmoAxis* GetGizmo() { return gizmo; }
	void TransformChange(D3DXVECTOR3 temp);

	void GizmoTransformMatchToBone();
	void GizmoTransformMatchToBuffer();

	ModelBuffer* Buffer() { return buffer; }

	GizmoAxis* GetSelectedGizmo();
	void SelectBoneUpdate();

	void SetWorld(D3DXMATRIX& mat);

	void CheckMaxMinVer(D3DXVECTOR3& max, D3DXVECTOR3& min);

	void TransformsCopy();

private:
	void BindMeshData();

private:
	class ModelBone* root;

	ModelBuffer* buffer;

	vector<Material*> materials;
	GizmoAxis* gizmo;

	vector<class ModelMesh*> meshes;
	vector<class ModelBone*> bones;

	map<wstring, vector<class ModelAnimClip*>> clips;
	//vector<class ModelAnimClip*> clips;

	D3DXVECTOR3 maxVer;
	D3DXVECTOR3 minVer;

	UINT selectIndex;

	class ModelCapsule* capsule;
	//vector<class ModelAnimation*> animes;
	Shader* shader2;
	Shader* shader3;

	vector<D3DXMATRIX> transforms;
};

class Models
{
public:
	friend class Model;

public:
	static void Create();
	static void Delete();

private:
	static void LoadMaterial(wstring file, vector<Material*>* materials);
	static void ReadMaterialData(wstring file);
	
	static void LoadMesh(wstring file, vector<class ModelBone*>* bones, vector<class ModelMesh*>* meshes);
	static void ReadMeshData(wstring file);

	static void LoadAnimation(wstring file, vector<class ModelAnimClip*>* clips);//, vector<class ModelMesh*>* meshes);
	static void ReadAnimData(wstring file);

private:
	static map<wstring, vector<Material*>> materialMap;
	
	struct MeshData
	{
		vector<class ModelBone*> Bones;
		vector<class ModelMesh*> Meshes;
	};
	static map<wstring, MeshData> meshDataMap;
	
	static map<wstring, vector<class ModelAnimClip*>> animClipMap;
};