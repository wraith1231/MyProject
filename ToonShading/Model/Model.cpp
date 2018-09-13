#include "stdafx.h"
#include "Model.h"

#include "ModelMesh.h"
#include "ModelBone.h"
#include "ModelAnimationClip.h"
#include "ModelCapsule.h"

#include "../Bounding/GizmoAxis.h"
#include "../Bounding/BoundingBox.h"

Model::Model()
{
	gizmo = new GizmoAxis();
	buffer = new ModelBuffer();
	selectIndex = 0;
	visible = true;
	capsule = NULL;
	//box = NULL;
	values = NULL;
	D3DXMatrixIdentity(&worldMat);
	maxVer = D3DXVECTOR3(-9999, -9999, -9999);
	minVer = D3DXVECTOR3(9999, 9999, 9999);
}

Model::~Model()
{
	SAFE_DELETE(buffer);
	SAFE_DELETE(gizmo);

	for (Material* material : materials)
		SAFE_DELETE(material);

	for (ModelMesh* mesh : meshes)
		SAFE_DELETE(mesh);

	for (map<wstring, vector<ModelAnimClip*>>::iterator iter = clips.begin(); iter != clips.end(); ++iter)
	{
		for (ModelAnimClip* clip : iter->second)
			SAFE_DELETE(clip);
	}
	clips.clear();

	for (ModelBone* bone : bones)
		SAFE_DELETE(bone);
}

void Model::Update()
{

}

void Model::Render()
{
	transforms.clear();
	CopyAbsoluteBoneTo(worldMat, transforms);

	for (ModelMesh* mesh : meshes)
	{
		int index = mesh->ParentBoneIndex();
		D3DXMATRIX transform = transforms[index];

		mesh->SetWorld(transform);
		mesh->Render();
	}
}

void Model::GizmoRender()
{
	bones[selectIndex]->Render();
}

void Model::CapsuleRender()
{
	if (capsule != NULL)
		capsule->Render();
}

void Model::PostRender()
{
	ImGui::Begin("Model Tree");

	{
		ModelTree(root);
	}

	ImGui::End();

}

void Model::ModelTree(ModelBone * bone)
{
	string sName = String::ToString(bone->name);
	string sNum = to_string(bone->index);

	const char* name = sName.c_str();
	const char* number = sNum.c_str();

	if (bone->childs.size() < 1)
	{
		ImGui::Indent();
		if (ImGui::Selectable(name))
		{/*
			if (bone->selected == false)
			{
				for (size_t i = 0; i < bones.size(); i++)
					bones[i]->selected = false;

				bone->selected = true;
			}*/
			selectIndex = bone->index;
		}

		ImGui::Unindent();
	}
	else
	{
		if (ImGui::TreeNodeEx(number, ImGuiTreeNodeFlags_OpenOnArrow))
		{
			ImGui::SameLine();
			if (ImGui::Selectable(name))
			{
				//if (bone->selected == false)
				//{
				//	for (size_t i = 0; i < bones.size(); i++)
				//		bones[i]->selected = false;
				//
				//	bone->selected = true;
				//}
				selectIndex = bone->index;
			}
			
			for (size_t i = 0; i < bone->childs.size(); i++)
			{
				ModelTree(bone->childs[i]);
			}
			ImGui::TreePop();
		}
	}
}

void Model::GizmoUpdate()
{
	for (ModelBone* bone : bones)
		bone->gizmo->Update();
}

void Model::CapsuleWorld(D3DXMATRIX mat)
{
	capsule->SetWorld(mat);
}

void Model::CapsuleScale(float scale)
{
	if (capsule != NULL)
		capsule->SetScale(scale);
}

Material * Model::GetMaterial(wstring name)
{
	for (Material* material : materials)
	{
		if (material->GetName() == name)
			return material;
	}

	return NULL;
}

ModelMesh * Model::Mesh(wstring name)
{
	for (ModelMesh* mesh : meshes)
	{
		if (mesh->name == name)
			return mesh;
	}

	return NULL;
}

ModelBone * Model::Bone(wstring name)
{
	for (ModelBone* bone : bones)
	{
		if (bone->name == name)
			return bone;
	}

	return NULL;
}

GizmoAxis * Model::GetSelectedGizmo()
{
	return bones[selectIndex]->gizmo;
}

void Model::SelectBoneUpdate()
{
	for (size_t i = 1; i < bones.size(); i++)
	{
		ModelBone* bone = bones[i];
		bone->gizmo->Update();
		bone->transform = bone->absoluteTransform * bone->gizmo->GetTempWorld();

	}
}

void Model::SetWorld(D3DXMATRIX& mat)
{
	worldMat = mat;
	for (ModelMesh* mesh : meshes)
		mesh->SetWorld(mat);
}
void Model::CheckMaxMinVer(D3DXVECTOR3 & max, D3DXVECTOR3 & min)
{
	max = maxVer;
	min = minVer;

	return;

	for (ModelMesh* mesh : meshes)
		mesh->CheckMaxMin(max, min);
}
void Model::TransformsCopy()
{
	transforms.clear();
	CopyAbsoluteBoneTo(worldMat, transforms);
}
void Model::VisibleUpdate()
{
	if (values != NULL)
	{
		visible = false;

		//box->Update(transforms[0]);
		D3DXVec3TransformCoord(&cullingCenter, &cullingOCenter, &worldMat);
		visible = values->ViewFrustum->CheckSphere(cullingCenter.x, cullingCenter.y, cullingCenter.z, cullingRadian);
		
		//if (values->ViewFrustum->Contain(box) == true)
		//	visible = true;
	}
}
//skin 있는 애들은 이거 안씀
void Model::CopyAbsoluteBoneTo(vector<D3DXMATRIX>& transforms)
{
	transforms.clear();
	transforms.assign(bones.size(), D3DXMATRIX());

	for (size_t i = 0; i < bones.size(); i++)
	{
		ModelBone* bone = bones[i];
		if (bone->parent != NULL)
		{
			int index = bone->parent->index;
			transforms[i] = bone->transform * transforms[index];
		}
		else
			transforms[i] = bone->transform;
	}

}

void Model::CopyAbsoluteBoneTo(D3DXMATRIX mat, vector<D3DXMATRIX>& transforms)
{
	transforms.clear();
	transforms.assign(bones.size(), D3DXMATRIX());

	for (size_t i = 0; i < bones.size(); i++)
	{
		ModelBone* bone = bones[i];
		if (bone->parent != NULL)
		{
			int index = bone->parent->index;
			transforms[i] = bone->transform * transforms[index];
		}
		else
			transforms[i] = bone->transform * mat;
	}
}

void Model::TransformChange(D3DXVECTOR3 temp)
{
	gizmo->PositionChange(temp);
}
void Model::GizmoTransformMatchToBone()
{
	for (ModelBone* bone : bones)
		bone->SetBoneTransformToGizmo();
}
void Model::GizmoTransformMatchToBuffer()
{
	for (size_t i = 1; i < bones.size(); i++)
	{
		bones[i]->gizmo->SetWorld(bones[i]->absoluteTransform * bones[i]->gizmo->GetWorld() * buffer->GetBoneMatrix(i - 1));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//Models
///////////////////////////////////////////////////////////////////////////////////////////////////


void Models::Create()
{
}

void Models::Delete()
{
	for (pair<wstring, vector<Material *>> temp : materialMap)
	{
		for (Material* material : temp.second)
			SAFE_DELETE(material);
	}

	for (pair<wstring, MeshData> temp : meshDataMap)
	{
		MeshData data = temp.second;

		for (ModelBone* bone : data.Bones)
			SAFE_DELETE(bone);

		for (ModelMesh* mesh : data.Meshes)
			SAFE_DELETE(mesh);
	}

}