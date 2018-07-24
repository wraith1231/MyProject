#pragma once

namespace Objects
{
	class BoundingBox;
	class BoundingFrustum;

	class BoundingSphere
	{
	private:
		class SphBuffer : public ShaderBuffer
		{
		public:
			SphBuffer() : ShaderBuffer(&Data, sizeof(Struct))
			{
				Data.Color = D3DXCOLOR(1, 1, 1, 1);
			}

			struct Struct
			{
				D3DXCOLOR Color;
			} Data;
		};
	public:
		BoundingSphere() {}
		BoundingSphere(D3DXVECTOR3 center, float radius);
		BoundingSphere(D3DXVECTOR3 center, D3DXVECTOR3 max, D3DXVECTOR3 min);
		~BoundingSphere();

		static Objects::BoundingSphere* CreateFromBoundingBox(class Objects::BoundingBox* box);
		static void CreateFromBoundingBox(class Objects::BoundingBox* box, BoundingSphere** sphere);

		static Objects::BoundingSphere* CreateFromPoint(vector<D3DXVECTOR3> points);
		static Objects::BoundingSphere* CreateFromPoint(D3DXVECTOR3* points);

		void SetVertex(UINT num);

		void Update();
		void Render();
		void Render(D3DXMATRIX mat);

		void SetWorld(D3DXMATRIX mat) { worldBuffer->SetMatrix(mat); }
		void SetColor(D3DXCOLOR col) { buffer->Data.Color = col; }

		void SetCenter(D3DXVECTOR3 cen) { center = cen; }

		D3DXVECTOR3 GetCenter() { return center; }
		float GetRadius() { return radius; }

		bool Intersects(class Objects::BoundingBox* box);

	private:

		WorldBuffer* worldBuffer;

		D3DXVECTOR3 center;
		float radius;

		vector<VertexColor> vertex;
		vector<UINT> index;

		ID3D11Buffer* vertexBuffer, *indexBuffer;

		SphBuffer* buffer;
		Shader* shader;

		D3DXVECTOR3 position;
	};
}