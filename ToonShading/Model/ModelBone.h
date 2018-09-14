#pragma once
#include "../Interfaces/ICloneable.h"

class GizmoAxis;

class ModelBone : public ICloneable
{
public:
	friend class Model;
	friend class Models;

private:
	ModelBone();
	~ModelBone();

public:
	int Index() { return index; }
	int ParentIndex() { return parentIndex; }

	ModelBone* Parent() { return parent; }

	void Transform(D3DXMATRIX& transform)
	{
		this->transform = transform;
	}

	D3DXMATRIX& Transform()
	{
		return transform;
	}

	void AbsoluteTransform(D3DXMATRIX& transform)
	{
		this->absoluteTransform = transform;
	}
	D3DXMATRIX& AbsoluteTransform()
	{
		return absoluteTransform;
	}

	UINT ChildCount() { return childs.size(); }
	ModelBone* Child(UINT index) { return childs[index]; }

	wstring Name() { return name; }

	void SetBoneTransformToGizmo();
public:
	void Clone(void** clone);

	void GizmoUpdate();
	void Render();

	class GizmoAxis* GetGizmo() { return gizmo; }

private:
	int   index;
	wstring name;

	int parentIndex;
	ModelBone* parent;

	vector<ModelBone *> childs;

	D3DXMATRIX transform;
	D3DXMATRIX absoluteTransform;

	GizmoAxis* gizmo;

	bool selected;
};