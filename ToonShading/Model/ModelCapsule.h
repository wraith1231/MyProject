#pragma once

class ModelCapsule
{
public:
	friend class Model;

public:
	bool CapsuleCollision(ModelCapsule* capsule);

public:
	ModelCapsule(D3DXVECTOR3 maxVer, D3DXVECTOR3 minVer);
	~ModelCapsule();

	void Render();

	void SetWorld(D3DXMATRIX mat);
	void SetScale(float scale) { this->scale = scale; }
	void SetDirection(D3DXVECTOR3 dir) { direction = dir; }
	void SetAbsolute(D3DXMATRIX mat) { absoluteTransform = mat; }

	static float GetVectorAngle(D3DXVECTOR3& v1, D3DXVECTOR3& v2);
	static bool LineCheck(D3DXVECTOR3& l1, D3DXVECTOR3& l2, float len);

private:
	vector<VertexColor> vertices;
	vector<UINT> indices;

	ID3D11Buffer* vertexBuffer, *indexBuffer;

	float height, radius;
	D3DXVECTOR3 center, direction;
	float scale;

	WorldBuffer* worldBuffer;
	D3DXMATRIX world;
	D3DXMATRIX absoluteTransform;

	Material* material;
};