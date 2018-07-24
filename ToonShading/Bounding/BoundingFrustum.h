#pragma once

namespace Objects
{
	class Plane;
	class Ray;
	class BoundingBox;
	class BoundingSphere;

	class BoundingFrustum
	{
	public:
		BoundingFrustum();
		BoundingFrustum(D3DXMATRIX matrix);
		~BoundingFrustum();

		void SetColor(D3DXCOLOR color) { buffer->Data.Color = color; }
		void Update();
		void Render();
		void Render(D3DXMATRIX mat);

		static D3DXVECTOR3 ComputeIntersection(Objects::Plane* plane, Objects::Ray* ray);
		static Objects::Ray* ComputeIntersection(Objects::Plane* p1, Objects::Plane* p2);

		void SetMatrix(D3DXMATRIX value);

		bool Contain(Objects::BoundingBox* box);
		bool Contain(Objects::BoundingSphere* sphere);

	private:
		class FruBuffer : public ShaderBuffer
		{
		public:
			FruBuffer() : ShaderBuffer(&Data, sizeof(Struct))
			{
				Data.Color = D3DXCOLOR(1, 1, 1, 1);
			}

			struct Struct
			{
				D3DXCOLOR Color;
			} Data;
		};

	private:
		const int cornerCount = 8;
		const int nearPlaneIndex = 0;
		const int farPlaneIndex = 1;
		const int leftPlaneIndex = 2;
		const int rightPlaneIndex = 3;
		const int topPlaneIndex = 4;
		const int bottomPlaneIndex = 5;
		const int numPlanes = 6;
	
	private:
		WorldBuffer* worldBuffer;

		D3DXMATRIX matrix;

		Objects::Plane* plane[6];
		D3DXVECTOR3 cornerArray[8];

		D3DXVECTOR3 position;

		Shader* shader;
		FruBuffer* buffer;

		vector<VertexColor> vertex;
		vector<UINT> index;

		ID3D11Buffer* vertexBuffer, *indexBuffer;
	};
}