/********************************************\
 *											*
 * MATRIX.H									*
 *											*
 * Mihail Ivanchev							*
 *											*
 * Matrix 4x4, 3x3 routines					*
 *											*
 * -----------------						*
 *											*
 * KISS - Keep it simple stupid				*
 *											*
\********************************************/

#ifndef _MATRIX_H_
#define _MATRIX_H_

//
// Structures
//

typedef struct
{
	union
	{
		struct
		{
			float _11,_12,_13,_14,
				  _21,_22,_23,_24,
				  _31,_32,_33,_34,
				  _41,_42,_43,_44;
		};
		struct
		{
			float m[4][4];
		};
	};
} Matrix4x4;

//
// Functions
//

void M4X4_Set(Matrix4x4 *m,float _11,float _12,float _13,float _14,
						   float _21,float _22,float _23,float _24,
						   float _31,float _32,float _33,float _34,
						   float _41,float _42,float _43,float _44);
void M4X4_Copy(Matrix4x4 *dst,const Matrix4x4 *src);

void M4X4_Zero(Matrix4x4 *m);
void M4X4_Identity(Matrix4x4 *m);

void M4X4_Add(Matrix4x4 *res,const Matrix4x4 *m1,const Matrix4x4 *m2);
void M4X4_Sub(Matrix4x4 *res,const Matrix4x4 *m1,const Matrix4x4 *m2);
void M4X4_Mul(Matrix4x4 *res,const Matrix4x4 *m1,const Matrix4x4 *m2);

void M4X4_Scale(Matrix4x4 *m,float sx,float sy,float sz);
void M4X4_Translate(Matrix4x4 *m,float tx,float ty,float tz);

#endif // _MATRIX_H_