#pragma once

class GizmoAxis
{
public:
	GizmoAxis(D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	~GizmoAxis();

	void Update();
	void Update(D3DXVECTOR3 position);
	void Render();

	bool MouseIntersect(D3DXVECTOR3 cam, D3DXVECTOR3 camDir, float& distance);

	void SetScale(D3DXVECTOR3 S) { s = S; }
	void SetRotate(D3DXVECTOR3 R) { r = R; }
	void SetTrans(D3DXVECTOR3 T) { t = T; }

	D3DXVECTOR3 GetScale() { return s; }
	D3DXVECTOR3 GetRotate() { return r; }
	D3DXVECTOR3 GetTrans() { return t; }

	void PositionChange(D3DXVECTOR3 change);

	void SetMode(UINT mo) { mode = mo; }
	UINT GetMode() { return mode; }

	void SetWorld(D3DXMATRIX mat);
	D3DXMATRIX GetWorld() { return worldBuffer->GetMatrix(); }
	D3DXMATRIX GetTempWorld() { return tempWorld; }

	void SetOriginWorld(D3DXMATRIX mat) { originTransform = mat; }
	D3DXMATRIX GetOriginWorld() { return originTransform; }

	static D3DXMATRIX MatrixInvert(D3DXMATRIX mat);

private:
	typedef VertexColor GizmoVertexType;

	bool xSelected, ySelected, zSelected;

	UINT mode;

	D3DXVECTOR3 s, r, t;

	Material* material;

	WorldBuffer* worldBuffer;
	D3DXMATRIX tempWorld;
	D3DXMATRIX originTransform;

	vector<GizmoVertexType> lineVertex;
	vector<UINT> lineIndex;

	vector<D3DXVECTOR3> xAxisVer;
	vector<D3DXVECTOR3> yAxisVer;
	vector<D3DXVECTOR3> zAxisVer;
	vector<UINT> axisIndex;

	ID3D11Buffer* lineVertexBuffer;
	ID3D11Buffer* lineIndexBuffer;

	ID3D11RasterizerState* rasterize[2];
	ID3D11DepthStencilState* depthStencilState[2];
};