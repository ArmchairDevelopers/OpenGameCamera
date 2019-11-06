#pragma once
// this file serves to basically replace everything we'd need from DirectX

// 32-bit color structure, broken down to RGBA
struct Color32 {
	byte r;
	byte g;
	byte b;
	byte a;
};

// basic Vector 2
struct Vec2 {
	float x;
	float y;
};

// basic Vector3
struct Vec3 {
	float x;
	float y;
	float z;
};

// Vector 4
struct Vec4 {
	float x;
	float y;
	float z;
	float w;

	// we need multiplication, addition, and subtraction operators
	Vec4 operator* (float value) {
		return { this->x * value, this->y * value, this->z * value, this->w * value };
	}
	Vec4 operator+ (Vec4 value) {
		return { this->x + value.x, this->y + value.y, this->z + value.z, this->w + value.w };
	}
	Vec4 operator- (Vec4 value) {
		return { this->x - value.x, this->y - value.y, this->z - value.z, this->w - value.w };
	}
};

// a Matrix4x4, just 4 vector4s
struct Matrix4x4 {
	Vec4 x;
	Vec4 y;
	Vec4 z;
	Vec4 o;
	Matrix4x4 operator= (Matrix4x4 value) {
		return { {value.x.x, value.x.y, value.x.z, value.x.w },
		{value.y.x, value.y.y, value.y.z, value.y.w},
		{value.z.x, value.z.y, value.z.z, value.z.w},
		{value.o.x, value.o.y, value.o.z, value.o.w} };
	}
};

typedef Matrix4x4 LinearTransform;