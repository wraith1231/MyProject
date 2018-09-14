#include "stdafx.h"
#include "ModelBone.h"

#include "../Bounding/GizmoAxis.h"

ModelBone::ModelBone()
{
	parent = NULL;
	gizmo = new GizmoAxis();
	selected = false;
}

ModelBone::~ModelBone()
{
}

void ModelBone::SetBoneTransformToGizmo()
{
	gizmo->SetWorld(absoluteTransform);
}

void ModelBone::Clone(void ** clone)
{
	ModelBone* bone = new ModelBone();

	bone->index = index;
	bone->name = name;
	
	bone->transform = transform;
	bone->absoluteTransform = absoluteTransform;
	bone->parentIndex = parentIndex;

	*clone = bone;
}

void ModelBone::GizmoUpdate()
{
	gizmo->Update();
}

void ModelBone::Render()
{
	gizmo->Render();
}
