#pragma once

namespace Objects
{
	class Rectangle
	{
	public:
		Rectangle();
		Rectangle(float x, float y, float width, float height);
		~Rectangle() {}

		float Bottom() { return y + height; }
		D3DXVECTOR2 Center()
		{
			return D3DXVECTOR2(x + width / 2, y + height / 2);
		}
		static Rectangle* GetEmpty() { return new Rectangle; }
		bool IsEmpty();

		float Left() { return x; }
		D3DXVECTOR2 Location()
		{
			return D3DXVECTOR2(x, y);
		}
		void Location(D3DXVECTOR2 point)
		{
			x = point.x;
			y = point.y;
		}

		float Right()
		{
			return x + width;
		}

		float Top()
		{
			return y;
		}

		bool Contains(float x, float y);
		void Contains(D3DXVECTOR2 value, bool& result);
		bool Contains(Rectangle* val);
		void Contains(Rectangle* value, bool& result);

		bool Equal(Rectangle* other);
		
		void Inflate(float horizontalAmount, float verticalAmount);
		static Objects::Rectangle* Intersect(Rectangle* value1, Rectangle* value2);
		static void Intersect(Rectangle* value1, Rectangle* value2, Rectangle** result);

		bool Intersects(Rectangle* value);
		void Intersects(Rectangle* value, bool& result);

		void Offset(D3DXVECTOR2 amount)
		{
			x += amount.x;
			y += amount.y;
		}
		void Offset(float offsetX, float offsetY)
		{
			x += offsetX;
			y += offsetY;
		}

		bool operator==(Rectangle* value);
		bool operator!=(Rectangle* value);

		static Objects::Rectangle* Union(Rectangle* a, Rectangle* b);
		static void Union(Rectangle* a, Rectangle* b, Rectangle** result);
		
	public: //º¯¼ö
		float x;
		float y;
		float width;
		float height;

	};
}