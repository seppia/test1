#include <math.h>
#include "main.h"
#include "baloons.h"


CBaloons::CBaloons()
{
	particlesCnt = 0;
	linksCnt = 0;
	baloonPos = D3DXVECTOR2(320, 240);
	baloonSpeed = D3DXVECTOR2(0.0f, 0.0f);
	baloonRadius = 30.0f;

	addParticle(baloonPos + D3DXVECTOR2(0, baloonRadius));
	for(int kk=0; kk<8; kk++)
	{
		addParticle(baloonPos + D3DXVECTOR2(0, baloonRadius + 15.0f + kk * 15.0f));
		addLink(kk, kk+1);
	}
}

void CBaloons::addParticle(D3DXVECTOR2 npos)
{
	particles[particlesCnt].pos = particles[particlesCnt].oldpos = npos;
	particles[particlesCnt].f = D3DXVECTOR2(0.0f, 0.0f);
	particlesCnt++;
}

void CBaloons::addLink(int part1, int part2)
{
	links[linksCnt].p1 = part1;
	links[linksCnt].p2 = part2;
	links[linksCnt].length = D3DXVec2Length(&(particles[part1].pos - particles[part2].pos));
	linksCnt++;			  
}

void CBaloons::updateBaloons(float dTime)
{
	//update Baloon Pos
	D3DXVECTOR2 mouse(g_ptMouse.x, g_ptMouse.y);
	D3DXVECTOR2 mb = baloonPos - mouse;
	float mblen = D3DXVec2Length(&mb);
	D3DXVec2Normalize(&mb, &mb);
	if(mblen < baloonRadius)
	{
		//baloonPos += mb * (baloonRadius - mblen);
		baloonSpeed += mb * (baloonRadius - mblen) * 100.0f;
	}

	baloonPos += baloonSpeed * dTime;
	baloonSpeed -= baloonSpeed * 2.8f * dTime;
	if(baloonSpeed.y > -60.0f)
		baloonSpeed.y -= 100.0f * dTime;

	if(baloonPos.x < 0.0f)
	{
		baloonPos.x = 0.0f;
		baloonSpeed.x = -baloonSpeed.x;
	}
	if(baloonPos.x > 640.0f)
	{
		baloonPos.x = 640.0f;
		baloonSpeed.x = -baloonSpeed.x;
	}
	if(baloonPos.y < 0.0f)
	{
		baloonPos.y = 0.0f;
		baloonSpeed.y = -baloonSpeed.y;
	}
	if(baloonPos.y > 480.0f)
	{
		baloonPos.y = 480.0f;
		baloonSpeed.y = -baloonSpeed.y;
	}

	//link tail to baloon
	particles[0].pos = baloonPos + D3DXVECTOR2(0.0f, baloonRadius);

	//resolve particles
	D3DXVECTOR2 vecGravity(0.0f, GRAVITY_Y);

	//1. compute forces
	for(int kk=0; kk<particlesCnt; kk++)
	{
		CParticle *part = &particles[kk];
		part->f = D3DXVECTOR2(0.0f, 0.0f);
		part->f += vecGravity;
	}
	
	//restrictiile le trateaza cu forte dar la baloane nu este cazul
	/*
	for(int kk=0; kk<linksCnt; kk++)
	{
		CLink *lnk = &links[kk];
		D3DXVECTOR2 p1p2 = particles[lnk->p2].pos - particles[lnk->p1].pos;
		float p1p2len = D3DXVec2Length(&p1p2);
		D3DXVec2Normalize(&p1p2, &p1p2);
		//restrictia actioneazaca un resort
		float Hterm = (p1p2len - lnk->length) * SPRING_Ks;	// var2 = Ks
				
		D3DXVECTOR2 springForce;				
		springForce = -(Hterm) * p1p2;	// Calc Force
		particles[lnk->p1].f += springForce;		
		particles[lnk->p2].f -= springForce;
	}
	*/
	//verlet integrate
	float dt_damped = pow(DEFAULT_DAMPING, dTime);
	float dt_square = dTime * dTime;
	D3DXVECTOR2 tmppos;
	for(int ii=0; ii < particlesCnt; ii++)
	{
		CParticle *state = &particles[ii];

		tmppos = state->pos;
		state->pos = state->pos + (state->pos - state->oldpos) * dt_damped + state->f * /*state->inverseMass*/ dt_square;
		state->oldpos = tmppos;
	}
	//constraints
	int maxIterations = SOLVER_MAX_ITERATIONS;
	bool solved = false;
	while((!solved) && (maxIterations > 0))
	{
		for(int kk=0; kk<linksCnt; kk++)
		{
			solved = true;
			CLink *lnk = &links[kk];
			D3DXVECTOR2 p1p2 = particles[lnk->p2].pos - particles[lnk->p1].pos;
			float p1p2len = D3DXVec2Length(&p1p2);
			if(p1p2len - lnk->length > 0.1f)
			{
				solved = false;
				D3DXVec2Normalize(&p1p2, &p1p2);
				particles[lnk->p1].pos += p1p2 * (p1p2len - lnk->length) * 0.5f;
				particles[lnk->p2].pos -= p1p2 * (p1p2len - lnk->length) * 0.5f;
			}
		}
		maxIterations--;
	}


}

void CBaloons::paintBaloons(Buffer *buff)
{
	buff->drawEllipse(baloonPos.x, baloonPos.y, baloonRadius, baloonRadius, 0, 255, 0);
	//paint links
	for(int kk=0; kk<linksCnt; kk++)
	{
		CLink *lnk = &links[kk];
		buff->line((int)particles[lnk->p1].pos.x, (int)particles[lnk->p1].pos.y, (int)particles[lnk->p2].pos.x, (int)particles[lnk->p2].pos.y, 255, 0, 0);	
	}

	for(int kk=0; kk<particlesCnt; kk++)
	{
		CParticle *prt = &particles[kk];
		buff->drawEllipse((int)prt->pos.x, (int)prt->pos.y, 2, 2, 0, 255, 0);
	}
}
