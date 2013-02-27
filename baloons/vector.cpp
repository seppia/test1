/********************************************\
 *											*
 * VECTOR.CPP								*
 *											*
 * Mihail Ivanchev							*
 *											*
 * Vector routines							*
 *											*
 * -----------------						*
 *											*
 * KISS - Keep it simple stupid				*
 *											*
\********************************************/

#include <math.h>
#include "vector.h"

void V3_Set(Vector3 *v,float x,float y,float z)
{
	v->x = x;
	v->y = y;
	v->z = z;
}

void V3_Copy(Vector3 *dst,const Vector3* src)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
}

void V3_Add(Vector3 *res,const Vector3 *v1,const Vector3 *v2)
{
	res->x = v1->x + v2->x;
	res->y = v1->y + v2->y;
	res->z = v1->z + v2->z;
}

void V3_Sub(Vector3 *res,const Vector3 *v1,const Vector3 *v2)
{
	res->x = v1->x - v2->x;
	res->y = v1->y - v2->y;
	res->z = v1->z - v2->z;
}

void V3_Mul(Vector3 *res,const Vector3 *v1,const Vector3 *v2)
{
	res->x = v1->x * v2->x;
	res->y = v1->y * v2->y;
	res->z = v1->z * v2->z;
}

void V3_Div(Vector3 *res,const Vector3 *v1,const Vector3 *v2)
{
	res->x = v1->x / v2->x;
	res->y = v1->y / v2->y;
	res->z = v1->z / v2->z;
}

void V3_Scale(Vector3 *res,const Vector3 *v,float s)
{
	res->x = v->x * s;
	res->y = v->y * s;
	res->z = v->z * s;
}

void V3_ScaleInv(Vector3 *res,const Vector3 *v,float s)
{
	if(s == 0)
	{
		res->x = 0;
		res->y = 0;
		res->z = 0;
		return;
	}
	
	float invS = 1.0f / s;

	res->x = v->x * s;
	res->y = v->y * s;
	res->z = v->z * s;
}

float V3_Magnitude(const Vector3 *v)
{
	return (float)sqrt(v->x * v->x +
					   v->y * v->y +
					   v->z * v->z);
}

void V3_Normalize(Vector3 *res,const Vector3 *v)
{
	float l = V3_Magnitude(v);
	
	// Only null vectors have null length

	if(l == 0) return;
		
	float invL = 1.0f / l;

	res->x = v->x * invL;
	res->y = v->y * invL;
	res->z = v->z * invL;
}

void V3_NormalizeSelf(Vector3 *v)
{
	float l = V3_Magnitude(v);
	
	// Only null vectors have null length

	if(l == 0) return;

	float invL = 1.0f / l;

	v->x = v->x * invL;
	v->y = v->y * invL;
	v->z = v->z * invL;
}

float V3_Dot(const Vector3 *v1,const Vector3 *v2)
{
	// TODO: Add implementation to this!!! I'm skipping it
	// because it's extremely DAMN SLOW!!!

	return 0;
}

float V3_DotNorm(const Vector3 *v1,const Vector3 *v2)
{
	return (v1->x * v2->x +
		    v1->y * v2->y +
			v1->z * v2->z);
}

void V3_Cross(Vector3 *res,const Vector3 *v1,const Vector3 *v2)
{
	res->x = v1->y * v2->z - v1->z * v2->y;
	res->y = v1->z * v2->x - v1->x * v2->z;
	res->z = v1->x * v2->y - v1->y * v2->x;
}
