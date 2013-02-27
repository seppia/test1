/********************************************\
 *											*
 * MATRIX.CPP									*
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

#include <math.h>
#include "matrix.h"

void M4X4_Set(Matrix4x4 *m,float _11,float _12,float _13,float _14,
						   float _21,float _22,float _23,float _24,
						   float _31,float _32,float _33,float _34,
						   float _41,float _42,float _43,float _44)
{
	m->_11 = _11;
	m->_12 = _12;
	m->_13 = _13;
	m->_14 = _14;
	m->_21 = _21;
	m->_22 = _22;
	m->_23 = _23;
	m->_24 = _24;
	m->_31 = _31;
	m->_32 = _32;
	m->_33 = _33;
	m->_34 = _34;
	m->_41 = _41;
	m->_42 = _42;
	m->_43 = _43;
	m->_44 = _44;
}

void M4X4_Copy(Matrix4x4 *dst,const Matrix4x4 *src)
{
	// TODO: Hmm....

	M4X4_Set(dst,src->_11,src->_12,src->_13,src->_14,
				 src->_21,src->_22,src->_23,src->_24,
				 src->_31,src->_32,src->_33,src->_34,
				 src->_41,src->_42,src->_43,src->_44);
}

void M4X4_Zero(Matrix4x4 *m)
{
	M4X4_Set(m,0,0,0,0,
			   0,0,0,0,
			   0,0,0,0,
			   0,0,0,0);
}

void M4X4_Identity(Matrix4x4 *m)
{
	M4X4_Set(m,1,0,0,0,
			   0,1,0,0,
			   0,0,1,0,
			   0,0,0,1);
}

void M4X4_Add(Matrix4x4 *res,const Matrix4x4 *m1,const Matrix4x4 *m2)
{
	long i,j;

	for(i = 0;i < 4;i++)
		for(j = 0;j < 4;j++)
			res->m[i][j] = m1->m[i][j] + m2->m[i][j];
}

void M4X4_Sub(Matrix4x4 *res,const Matrix4x4 *m1,const Matrix4x4 *m2)
{
	long i,j;

	for(i = 0;i < 4;i++)
		for(j = 0;j < 4;j++)
			res->m[i][j] = m1->m[i][j] - m2->m[i][j];
}

void M4X4_Mul(Matrix4x4 *res,const Matrix4x4 *m1,const Matrix4x4 *m2)
{
	float	e1,e2,e3,e4;
	long	i;

	for(i = 0;i < 4;i++)
	{
		e1 = m1->m[i][0];
		e2 = m1->m[i][1];
		e3 = m1->m[i][2];
		e4 = m1->m[i][3];

		res->m[i][0] = e1 * m2->_11 + e2 * m2->_21 + e3 * m2->_31 + e4 * m2->_41;
		res->m[i][1] = e1 * m2->_12 + e2 * m2->_22 + e3 * m2->_32 + e4 * m2->_42;
		res->m[i][2] = e1 * m2->_13 + e2 * m2->_23 + e3 * m2->_33 + e4 * m2->_43;
		res->m[i][3] = e1 * m2->_14 + e2 * m2->_24 + e3 * m2->_34 + e4 * m2->_44;
	}
}

void M4X4_Scale(Matrix4x4 *m,float sx,float sy,float sz)
{
	Matrix4x4 temp,mat;
	M4X4_Copy(&temp,m);
	M4X4_Identity(&mat);
	mat._11 = sx;
	mat._22 = sy;
	mat._33 = sz;
	M4X4_Mul(m,&mat,&temp);
}

void M4X4_Translate(Matrix4x4 *m,float tx,float ty,float tz)
{
	Matrix4x4 temp,mat;
	M4X4_Copy(&temp,m);
	M4X4_Identity(&mat);
	mat._41 = tx;
	mat._42 = ty;
	mat._43 = tz;
	M4X4_Mul(m,&mat,&temp);
}
