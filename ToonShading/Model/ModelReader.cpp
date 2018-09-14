#include "stdafx.h"
#include "Model.h"

#include "ModelMesh.h"
#include "ModelMeshPart.h"
#include "ModelBone.h"
#include "ModelAnimationClip.h"
#include "ModelCapsule.h"

#include "../Utilities/BinaryFile.h"
#include "../Bounding/BoundingBox.h"

void Model::ReadMaterial(wstring file)
{
	Models::LoadMaterial(file, &materials);

}

void Model::ReadMesh(wstring file)
{
	Models::LoadMesh(file, &bones, &meshes);

	BindMeshData();

	//GizmoTransformMatchToBone();
}

void Model::ReadAnim(wstring file, wstring animName)
{
	Models::LoadAnimation(file, &clips[animName]);
}


void Model::BindMeshData()
{
	for (ModelMesh* mesh : meshes)
	{
		for (ModelMeshPart* part : mesh->meshParts)
			part->material = GetMaterial(part->materialName);
	}

	this->root = bones[0];
	for (ModelBone* bone : bones)
	{
		if (bone->parentIndex > -1)
		{
			bone->parent = bones[bone->parentIndex];
			bone->parent->childs.push_back(bone);
		}
		else
		{
			bone->parent = NULL;
		}
	}

	for (ModelMesh* mesh : meshes)
	{
		mesh->parentBone = bones[mesh->parentBoneIndex];
		mesh->Binding();
		//if (mesh->name == L"Paladin_J_Nordstrom")
		{
			mesh->CheckMaxMin(maxVer, minVer);
		}
	}

	capsule = new ModelCapsule(maxVer, minVer);
	//box = new Objects::BoundingBox(maxVer, minVer);
	cullingCenter = cullingOCenter = (maxVer + minVer) * 0.5f;
	cullingRadian = D3DXVec3Length(&(maxVer - minVer));

}

///////////////////////////////////////////////////////////////////////////////////////////////////
//Models
//
///////////////////////////////////////////////////////////////////////////////////////////////////

map<wstring, vector<Material*>> Models::materialMap = map<wstring, vector<Material*>>();
void Models::LoadMaterial(wstring file, vector<Material*>* materials)
{
	if (materialMap.count(file) < 1)
	{
		ReadMaterialData(file);
	}

	for (Material* material : materialMap[file])
	{
		Material* temp = NULL;
		material->Clone((void**)&temp);
		
		materials->push_back(temp);
	}
}

void Models::ReadMaterialData(wstring file)
{
	vector<Material*> srcMaterial;

	Json::Value root;
	Json::ReadData(file, &root);

	Json::Value::iterator iter = root.begin();
	for (; iter != root.end(); iter++)
	{
		Json::Value value = (*iter);
		Material* material = new Material();

		string name;
		Json::GetValue(value, "Name", name);
		material->SetName(String::ToWString(name));

		Json::GetValue(value, "ShaderName", name);
		if (name.length() > 0)
			material->SetShader(Shaders + String::ToWString(name));


		D3DXCOLOR color;
		Json::GetValue(value, "Ambient", color);
		material->SetAmbient(color);

		Json::GetValue(value, "Diffuse", color);
		material->SetDiffuse(color);

		Json::GetValue(value, "Emissive", color);
		material->SetEmissive(color);

		Json::GetValue(value, "Specular", color);
		material->SetSpecular(color);


		float shininess;
		Json::GetValue(value, "Shininess", shininess);
		material->SetShininess(shininess);


		string textureFile;

		string directory = Path::GetDirectoryName(String::ToString(file));

		Json::GetValue(value, "DiffuseFile", textureFile);
		if (textureFile.length() > 0)
			material->SetDiffuseMap(directory + textureFile);

		Json::GetValue(value, "SpecularFile", textureFile);
		if (textureFile.length() > 0)
			material->SetSpecularMap(directory + textureFile);

		Json::GetValue(value, "EmissiveFile", textureFile);
		if (textureFile.length() > 0)
			material->SetEmissiveMap(directory + textureFile);

		Json::GetValue(value, "NormalFile", textureFile);
		if (textureFile.length() > 0)
			material->SetNormalMap(directory + textureFile);

		srcMaterial.push_back(material);
	}

	materialMap[file] = srcMaterial;

}

map<wstring, Models::MeshData> Models::meshDataMap = map<wstring, Models::MeshData>();
void Models::LoadMesh(wstring file, vector<class ModelBone*>* bones, vector<class ModelMesh*>* meshes)
{
	if (meshDataMap.count(file) < 1)
		ReadMeshData(file);

	MeshData data = meshDataMap[file];
	for (ModelBone* bone : data.Bones)
	{
		ModelBone* temp = NULL;
		bone->Clone((void**)&temp);

		bones->push_back(temp);
	}

	for (ModelMesh* mesh : data.Meshes)
	{
		ModelMesh* temp = NULL;
		mesh->Clone((void**)&temp);

		meshes->push_back(temp);
	}
}

void Models::ReadMeshData(wstring file)
{
	BinaryReader* r = new BinaryReader();
	r->Open(file);

	UINT count = 0;

	vector<ModelBone*> bones;
	vector<ModelMesh*> meshes;

	count = r->UInt();
	for (UINT i = 0; i < count; i++)
	{
		ModelBone* bone = new ModelBone();
		bone->index = r->Int();
		bone->name = String::ToWString(r->String());
		bone->parentIndex = r->Int();
		bone->transform = r->Matrix();
		bone->absoluteTransform = r->Matrix();

		bones.push_back(bone);
	}

	count = r->UInt();
	for (UINT i = 0; i < count; i++)
	{
		ModelMesh* mesh = new ModelMesh();

		mesh->name = String::ToWString(r->String());
		int parentBoneIndex = r->Int();
		for (ModelBone* bone : bones)
		{
			if (parentBoneIndex == bone->index)
			{
				mesh->parentBoneIndex = parentBoneIndex;
				mesh->parentBone = bone;
				break;
			}
		}

		UINT partCount = r->UInt();
		for (UINT p = 0; p < partCount; p++)
		{
			ModelMeshPart* part = new ModelMeshPart();
			part->parent = mesh;

			wstring materialName = String::ToWString(r->String());
			part->materialName = materialName;

			//vertex read
			{
				UINT c = r->UInt();
				part->vertices.assign(c, ModelVertexType());

				void* ptr = (void*)&(part->vertices[0]);
				r->Byte(&ptr, sizeof(ModelVertexType) * c);
			}
			//index read
			{
				UINT c = r->UInt();
				part->indices.assign(c, UINT());

				void* ptr = (void*)&(part->indices[0]);
				r->Byte(&ptr, sizeof(UINT) * c);
			}
			mesh->meshParts.push_back(part);
		} //for p

		meshes.push_back(mesh);
	} // for i

	r->Close();
	SAFE_DELETE(r);

	MeshData data;
	data.Bones.assign(bones.begin(), bones.end());
	data.Meshes.assign(meshes.begin(), meshes.end());

	meshDataMap[file] = data;
}

map<wstring, vector<ModelAnimClip*>> Models::animClipMap;
void Models::LoadAnimation(wstring file, vector<class ModelAnimClip*>* clips)//, vector<class ModelMesh*>* meshes)
{
	if (animClipMap.count(file) < 1)
		ReadAnimData(file);

	vector<ModelAnimClip*> data = animClipMap[file];

	for (ModelAnimClip* clip : data)
	{
		ModelAnimClip* temp = NULL;
		clip->Clone((void**)&temp);

		clips->push_back(clip);
	}
}

void Models::ReadAnimData(wstring file)
{
	BinaryReader* r = new BinaryReader();
	r->Open(file);

	UINT count = r->UInt();
	vector<ModelAnimClip *> clips;
	for (UINT i = 0; i < count; i++)
	{
		ModelAnimClip* clip = new ModelAnimClip();
		clip->name = String::ToWString(r->String());

		clip->totalFrame = r->Int();
		clip->frameRate = r->Float();
		clip->defaltFrameRate = clip->frameRate;

		UINT frameCount = r->UInt();
		for (UINT frame = 0; frame < frameCount; frame++)
		{
			ModelKeyframe* keyframe = new ModelKeyframe();
			keyframe->BoneName = String::ToWString(r->String());


			UINT count = r->UInt();
			keyframe->Datas.assign(count, ModelKeyframeData());

			void* ptr = (void *)&(keyframe->Datas[0]);
			r->Byte(&ptr, sizeof(ModelKeyframeData) * count);

			clip->keyframeMap[keyframe->BoneName] = keyframe;
		}//for(frame)

		clips.push_back(clip);
	}

	r->Close();
	SAFE_DELETE(r);

	animClipMap[file] = clips;
}