#pragma once

namespace Objects
{
	class BoundingBox;

	class Ray
	{
		private:
		class RayBuffer : public ShaderBuffer
		{
		public:
			RayBuffer() : ShaderBuffer(&Data, sizeof(Struct))
			{
				Data.Color = D3DXCOLOR(0, 1, 0, 1);
			}

			struct Struct
			{
				D3DXCOLOR Color;
			} Data;
		};

	public:
		Ray();
		Ray(D3DXVECTOR3 pos, D3DXVECTOR3 dir);
		~Ray() {}

		void Update(D3DXVECTOR3 pos, D3DXVECTOR3 dir);
		void Render();

		bool Intersects(BoundingBox* box);

		void SetColor(D3DXCOLOR col);

		static class Objects::Ray* ConvertMouseToRay(D3DXVECTOR3 mouse, ExecuteValues* value);

	public:
		D3DXVECTOR3 Position;
		D3DXVECTOR3 Direction;

		Shader* shader;
		WorldBuffer* worldBuffer;

		vector<VertexColor> vertex;
		vector<UINT> index;

		RayBuffer* buffer;
		ID3D11Buffer* vertexBuffer, *indexBuffer;
	};
}