/********************************************\
 *											*
 * VECTOR.H									*
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

#ifndef _VECTOR_H_
#define _VECTOR_H_

//
// Structures
//

typedef struct
{
	union
	{
		struct { float v[3]; };
		struct { float x,y,z; };
	};
} Vector3;

//
// Functions
//

void V3_Set(Vector3 *v,float x,float y,float z);
void V3_Copy(Vector3 *dst,const Vector3* src);

void V3_Add(Vector3 *res,const Vector3 *v1,const Vector3 *v2);
void V3_Sub(Vector3 *res,const Vector3 *v1,const Vector3 *v2);
void V3_Mul(Vector3 *res,const Vector3 *v1,const Vector3 *v2);
void V3_Div(Vector3 *res,const Vector3 *v1,const Vector3 *v2);
void V3_Scale(Vector3 *res,const Vector3 *v,float s);
void V3_ScaleInv(Vector3 *res,const Vector3 *v,float s);

float V3_Magnitude(const Vector3 *v);
void V3_Normalize(Vector3 *res,const Vector3 *v);
void V3_NormalizeSelf(Vector3 *v);

float V3_Dot(const Vector3 *v1,const Vector3 *v2);
float V3_DotNorm(const Vector3 *v1,const Vector3 *v2);
void V3_Cross(Vector3 *res,const Vector3 *v1,const Vector3 *v2);

#endif _VECTOR_H_