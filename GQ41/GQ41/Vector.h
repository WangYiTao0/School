#pragma once

class Vector
{
public:
	Vector()
	{
		x = y = z = 0;
	}
	Vector(float X, float Y, float Z)
	{
		x = X;
		y = Y;
		z = Z;
	}
	Vector(float XYZ[3])
	{
		x = XYZ[0];
		y = XYZ[1];
		z = XYZ[2];
	}
	Vector(const class Point& p);

public:
	float Length() const;
	float LengthSqr() const;

	Vector operator-() const;

	Vector operator+(const Vector& v) const;
	Vector operator-(const Vector& v) const;

	Vector operator*(float s) const;
	Vector operator/(float s) const;

	Vector Normalized() const;
	void Normalize();

	float  Dot(const Vector& v) const;
	Vector Cross(const Vector& v) const;

public:
	union {
		struct {
			float x, y, z;
		};
		float v[3];
	};
};

inline const Vector operator*(float s, const Vector& v)
{
	return Vector(s * v.x, s * v.y, s * v.z);
}

inline const Vector operator/(float s, const Vector& v)
{
	return Vector(s / v.x, s / v.y, s / v.z);
}

class Point
{
public:
	Point() {}
	Point(float X, float Y, float Z)
	{
		x = X;
		y = Y;
		z = Z;
	}

	Point(const Vector& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	Point operator+(const Vector& v) const;
	Point operator-(const Vector& v) const;

	float x, y, z;
};
