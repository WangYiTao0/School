#pragma once
#include "Vector.h"

/// <summary>
/// 
/// </summary>
/// <param name="v1"> v2 project on v1 </param>
/// <param name="v2"> </param>
/// <returns></returns>
Vector VectorProjection(Vector v1, Vector v2)
{
	Vector b1 = v2.Length() / v1.Length() * v1;

	return b1;
}

Vector GetPlaneNormalFormTrangle(Vector A, Vector B, Vector C)
{
	return (B - A) .Cross(C - A);
}

bool SameSide(Vector A, Vector B, Vector C, Vector P)
{
	Vector AB = B - A;
	Vector AC = C - A;
	Vector AP = P - A;

	Vector v1 = AB.Cross(AC);
	Vector v2 = AB.Cross(AP);

	// v1 and v2 should point to the same direction
	return v1.Dot(v2) >= 0;
}


bool PointTrangleIntersection(Vector A, Vector B, Vector C, Vector P)
{
	return SameSide(A, B, C, P) &&
		SameSide(B, C, A, P) &&
		SameSide(C, A, B, P);
}

bool LinePlaneIntersection(const Vector& n, const Vector& c, const Vector& x0, const Vector& x1, Vector& vecIntersection, float& flFraction)
{
	// n - plane normal
	// c - any point in the plane
	// x0 - the beginning of our line
	// x1 - the end of our line

	Vector v = x1 - x0;
	Vector w = c - x0;

	float k = w.Dot(n) / v.Dot(n);

	vecIntersection = x0 + k * v;

	flFraction = k;

	return k >= 0 && k <= 1;
}

float Bary_Centric_Coordinate(const Vector& A, const Vector& B, const Vector& C, const Vector& I)
{
	Vector AB = B - A;
	Vector CB = B - C;
	Vector AI = I - A;

	Vector v = AB - VectorProjection(CB, AB);

	float a = 1.0- (v.Dot(AI) / v.Dot(AB));
	return a ;
}

bool LineTrangleIntersection(const Vector& l1, const Vector& l2, const Vector& A, const Vector& B, const Vector& C)
{
	Vector I;

	return 0 < Bary_Centric_Coordinate(A, B, C, I) < 1 && 
		0 < Bary_Centric_Coordinate( B, C, A, I) < 1;

}

bool IntersectTriangle(const D3DXVECTOR3& orig, const D3DXVECTOR3& dir,
	D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2,
	FLOAT* t, FLOAT* u, FLOAT* v)
{
	// Find vectors for two edges sharing vert0
	D3DXVECTOR3 edge1 = v1 - v0;
	D3DXVECTOR3 edge2 = v2 - v0;

	// Begin calculating determinant - also used to calculate U parameter
	D3DXVECTOR3 pvec;
	D3DXVec3Cross(&pvec, &dir, &edge2);

	// If determinant is near zero, ray lies in plane of triangle
	FLOAT det = D3DXVec3Dot(&edge1, &pvec);

	D3DXVECTOR3 tvec;
	if (det > 0)
	{
		tvec = orig - v0;
	}
	else
	{
		tvec = v0 - orig;
		det = -det;
	}

	if (det < 0.0001f)
		return FALSE;

	// Calculate U parameter and test bounds
	*u = D3DXVec3Dot(&tvec, &pvec);
	if (*u < 0.0f || *u > det)
		return FALSE;

	// Prepare to test V parameter
	D3DXVECTOR3 qvec;
	D3DXVec3Cross(&qvec, &tvec, &edge1);

	// Calculate V parameter and test bounds
	*v = D3DXVec3Dot(&dir, &qvec);
	if (*v < 0.0f || *u + *v > det)
		return FALSE;

	// Calculate t, scale parameters, ray intersects triangle
	*t = D3DXVec3Dot(&edge2, &qvec);
	FLOAT fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return TRUE;
}