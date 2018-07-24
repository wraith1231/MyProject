#include "stdafx.h"
#include "ModelMesh.h"

#include "ModelMeshPart.h"
#include "ModelBone.h"

ModelMesh::ModelMesh()
{
	worldBuffer = new WorldBuffer;
	parentBone = NULL;
	parentBoneIndex = 0;
}

ModelMesh::~ModelMesh()
{
	for (ModelMeshPart* part : meshParts)
		SAFE_DELETE(part);

	SAFE_DELETE(worldBuffer);
}

void ModelMesh::Binding()
{
	for (ModelMeshPart* part : meshParts)
		part->Binding();
}

void ModelMesh::Clone(void ** clone)
{
	ModelMesh* mesh = new ModelMesh();

	mesh->name = name;
	mesh->parentBoneIndex = parentBoneIndex;

	for (ModelMeshPart* part : meshParts)
	{
		ModelMeshPart* temp = NULL;
		part->Clone((void**)&temp);

		temp->parent = mesh;
		mesh->meshParts.push_back(temp);
	}

	*clone = mesh;
}

void ModelMesh::Render()
{
	worldBuffer->SetVSBuffer(1);

	for (ModelMeshPart* part : meshParts)
		part->Render();
}

void ModelMesh::CheckMaxVer(D3DXVECTOR3 & max)
{
	for (ModelMeshPart* part : meshParts)
		part->CheckMaxVer(max);
}

void ModelMesh::CheckMinVer(D3DXVECTOR3 & min)
{
	for (ModelMeshPart* part : meshParts)
		part->CheckMinVer(min);
}

void ModelMesh::CheckMaxMin(D3DXVECTOR3 & max, D3DXVECTOR3 & min)
{
	for (ModelMeshPart* part : meshParts)
		part->CheckMaxMin(max, min);
}
