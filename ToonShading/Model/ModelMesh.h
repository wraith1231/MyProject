#pragma once
#include "../Interfaces/ICloneable.h"

class ModelMesh : public ICloneable
{
public:
	friend class Model;
	friend class Models;

private:
	ModelMesh();
	~ModelMesh();

	void Binding();

public:
	void Clone(void** clone);
	
public:
	void SetWorld(D3DXMATRIX& world)
	{
		worldBuffer->SetMatrix(world);
	}

	wstring Name() { return name; }

	int ParentBoneIndex() { return parentBoneIndex; }
	class ModelBone* ParentBone() { return parentBone; }

	void Render();

	void CheckMaxVer(D3DXVECTOR3& max);
	void CheckMinVer(D3DXVECTOR3& min);

	void CheckMaxMin(D3DXVECTOR3& max, D3DXVECTOR3& min);

private:
	wstring name;
	int parentBoneIndex;
	class ModelBone* parentBone;

	WorldBuffer* worldBuffer;

	vector<class ModelMeshPart*> meshParts;	//child
};