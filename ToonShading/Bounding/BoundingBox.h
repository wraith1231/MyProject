#pragma once

namespace Objects
{
	class Plane;
	class Ray;

	class BoundingBox
	{
	private:
		class BoxBuffer : public ShaderBuffer
		{
		public:
			BoxBuffer() : ShaderBuffer(&Data, sizeof(Struct))
			{
				Data.Color = D3DXCOLOR(0, 0, 1, 1);
			}

			struct Struct
			{
				D3DXCOLOR Color;
			} Data;
		};

	public:
		BoundingBox(D3DXVECTOR3 max, D3DXVECTOR3 min);
		~BoundingBox();

		static void CreateFromPoints(vector<D3DXVECTOR3> points, Objects::BoundingBox** box);
		static void CreateFromPoints(D3DXVECTOR3* points, Objects::BoundingBox** box);

		static void VectorMin(D3DXVECTOR3& v1, D3DXVECTOR3& v2, OUT D3DXVECTOR3& v3);
		static void VectorMax(D3DXVECTOR3& v1, D3DXVECTOR3& v2, OUT D3DXVECTOR3& v3);

		void SetColor(D3DXCOLOR color) { buffer->Data.Color = color; }

		void SetMatrix(D3DXMATRIX mat) { worldBuffer->SetMatrix(mat); }

		void Update();
		void Update(D3DXMATRIX world);
		void Update(D3DXVECTOR3 pos);
		void Render();
		void Render(D3DXMATRIX mat);

		D3DXVECTOR3 GetMax() { return max; }
		D3DXVECTOR3 GetMin() { return min; }

		bool Intersects(Objects::BoundingBox* box);
		bool Intersects(Objects::Plane* plane);
		bool Intersects(D3DXPLANE plane);
		bool Intersects(Objects::Ray* ray);

	private:
		WorldBuffer* worldBuffer;

		D3DXVECTOR3 oMax, oMin;
		D3DXVECTOR3 max, min;

		vector<VertexColor> vertex;
		vector<UINT> index;
		
		Shader* shader;
		BoxBuffer* buffer;

		D3DXVECTOR3 position;

		ID3D11Buffer* vertexBuffer, *indexBuffer;
	};
}