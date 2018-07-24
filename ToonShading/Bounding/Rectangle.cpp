#include "stdafx.h"
#include "Rectangle.h"

Objects::Rectangle::Rectangle()
{
	x = 0;
	y = 0;
	width = 0;
	height = 0;
}

Objects::Rectangle::Rectangle(float x, float y, float width, float height)
	: x(x), y(y), width(width), height(height)
{
	
}

bool Objects::Rectangle::IsEmpty()
{
	if (fabs(x) < 0.0001f && fabs(y) < 0.0001f
		&& fabs(width) < 0.0001f && fabs(height) < 0.0001f)
		return true;

	return false;
}

bool Objects::Rectangle::Contains(float x, float y)
{
	if (this->x > x || x >= this->x + this->width || this->y > y)
		return false;
	return y < this->y + this->height;
}

void Objects::Rectangle::Contains(D3DXVECTOR2 value, bool & result)
{
	if (this->x > value.x || value.x >= this->x + this->width || this->y > value.y)
	{
		result = false;
		return;
	}

	result = value.y < this->y + this->height;
}

bool Objects::Rectangle::Contains(Rectangle * val)
{
	if (this->x > val->x || val->x + val->width > this->x + this->width || this->y > val->y)
		return false;

	return val->y + val->height <= this->y + this->height;
}

void Objects::Rectangle::Contains(Rectangle * value, bool & result)
{
	result = (this->x > value->x || value->x + value->width > this->x + this->width || this->y > value->y ? false : value->y + value->height <= this->y + this->height);
}

bool Objects::Rectangle::Equal(Rectangle * other)
{
	if (fabs(this->x - other->x) < 0.0001f ||
		fabs(this->y - other->y) < 0.0001f ||
		fabs(this->width - other->width) < 0.0001f)
		return false;

	return fabs(this->height - other->height) < 0.0001f;
}

void Objects::Rectangle::Inflate(float horizontalAmount, float verticalAmount)
{
	x -= horizontalAmount;
	y -= verticalAmount;

	Rectangle* wid = this;
	wid->width = wid->width + horizontalAmount * 2;
	Rectangle* hei = this;
	hei->height = hei->height + verticalAmount * 2;
}

Objects::Rectangle * Objects::Rectangle::Intersect(Rectangle * value1, Rectangle * value2)
{
	Rectangle* rectangle = new Rectangle();

	float x = value1->x + value1->width;
	float num = value2->x + value2->width;

	float y = value1->y + value1->height;
	float y1 = value2->y + value2->height;

	float num1 = (value1->x > value2->x) ? value1->x : value2->x;
	float num2 = (value1->y > value2->y) ? value1->y : value2->y;
	float num3 = (x < num ? x : num);
	float num4 = (y < y1 ? y : y1);

	if (num3 <= num1 || num4 <= num2)
	{
		rectangle->x = 0.0f;
		rectangle->y = 0.0f;
		rectangle->width = 0.0f;
		rectangle->height = 0.0f;
	}
	else
	{
		rectangle->x = num1;
		rectangle->y = num2;
		rectangle->width = num3 - num1;
		rectangle->height = num4 - num2;
	}

	return rectangle;
}

void Objects::Rectangle::Intersect(Rectangle * value1, Rectangle * value2, Rectangle ** result)
{
	*result = new Rectangle();

	float x = value1->x + value1->width;
	float num = value2->x + value2->width;

	float y = value1->y + value1->height;
	float y1 = value2->y + value2->height;

	float num1 = (value1->x > value2->x) ? value1->x : value2->x;
	float num2 = (value1->y > value2->y) ? value1->y : value2->y;
	float num3 = (x < num ? x : num);
	float num4 = (y < y1 ? y : y1);

	if (num3 <= num1 || num4 <= num2)
	{
		(*result)->x = 0.0f;
		(*result)->y = 0.0f;
		(*result)->width = 0.0f;
		(*result)->height = 0.0f;
	}
	else
	{
		(*result)->x = num1;
		(*result)->y = num2;
		(*result)->width = num3 - num1;
		(*result)->height = num4 - num2;
	}
}

bool Objects::Rectangle::Intersects(Rectangle * value)
{
	if (value->x >= this->x + this->width || this->x >= value->x + value->width || value->y >= this->y + this->height)
		return false;

	return this->y < value->y + value->height;
}

void Objects::Rectangle::Intersects(Rectangle * value, bool & result)
{
	result = Intersects(value);
}

bool Objects::Rectangle::operator==(Rectangle * value)
{
	return Equal(value);
}

bool Objects::Rectangle::operator!=(Rectangle * value)
{
	return !Equal(value);
}

Objects::Rectangle * Objects::Rectangle::Union(Rectangle * a, Rectangle * b)
{
	Rectangle* rect = new Rectangle();

	float x = a->x + a->width;
	float num = b->x + b->width;

	float y = a->y + a->height;
	float y1 = b->y + b->height;

	float num1 = (a->x < b->x ? a->x : b->x);
	float num2 = (a->y < b->y ? a->y : b->y);
	float num3 = (x > num ? x : num);
	float num4 = (y > y1 ? y : y1);

	rect->x = num1;
	rect->y = num2;
	rect->width = num3 - num1;
	rect->height = num4 - num2;

	return rect;
}

void Objects::Rectangle::Union(Rectangle * a, Rectangle * b, Rectangle ** result)
{
	*result = new Rectangle();

	float x = a->x + a->width;
	float num = b->x + b->width;

	float y = a->y + a->height;
	float y1 = b->y + b->height;

	float num1 = (a->x < b->x ? a->x : b->x);
	float num2 = (a->y < b->y ? a->y : b->y);
	float num3 = (x > num ? x : num);
	float num4 = (y > y1 ? y : y1);

	(*result)->x = num1;
	(*result)->y = num2;
	(*result)->width = num3 - num1;
	(*result)->height = num4 - num2;
}

