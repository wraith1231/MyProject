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
	//gizmo->SetOriginWorld(absoluteTransform);
	gizmo->SetWorld(absoluteTransform);
}

void ModelBone::Clone(void ** clone)
{
	ModelBone* bone = new ModelBone();

	bone->index = index;
	bone->name = name;
	
	bone->transform = transform;
	bone->absoluteTransform = absoluteTransform;
	bone->oriTransform = oriTransform;
	bone->parentIndex = parentIndex;

	*clone = bone;
}

void ModelBone::GizmoUpdate()
{
	//gizmo->SetOriginWorld(transform);
	gizmo->Update();
}

void ModelBone::Render()
{
	//if(name == L"R_Ball")
	//if(selected == true)
		gizmo->Render();
}
