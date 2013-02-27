#ifndef _BALOONS_H_
#define _BALOONS_H_

#include "buffer.h"


#define SPRING_EPSILON  0.00001f				// ERROR TERM
#define MIN_TIME_EPSILON  0.000001f
#define DEFAULT_DAMPING		0.002f   //cam trebuie sa fie fixa valoarea asta pe 0.5f
// Velocity Threshold that decides between Static and Kinetic Friction
#define STATIC_THRESHOLD	0.03f

#define DEPTH_EPSILON 1.0f
#define GRAVITY_Y 1000.0f
#define SPRING_Ks	0.01f

#define SOLVER_MAX_ITERATIONS 3

typedef struct _tagParticle {
	D3DXVECTOR2 pos;
	D3DXVECTOR2 oldpos;
	D3DXVECTOR2 f; //forta care actioneaza pe particula
} CParticle;

typedef struct _tagLink {
	int p1, p2; //indexurile particulelor pe care le leaga
	float length; //lungimea legaturii
} CLink;

#define MAX_PARTICLES 20
#define MAX_LINKS 20

class CBaloons {
public:
	D3DXVECTOR2 baloonPos, baloonSpeed;
	float baloonRadius;

	CParticle particles[MAX_PARTICLES];
	CLink links[MAX_LINKS];

	int particlesCnt, linksCnt;

	CBaloons();

	void addParticle(D3DXVECTOR2 npos);
	void addLink(int part1, int part2);

	void updateBaloons(float dTime);
	void paintBaloons(Buffer *buff);
};

#endif _BALOONS_H_