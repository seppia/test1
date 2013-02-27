
#ifndef _MAIN_H_
#define _MAIN_H_

#ifndef _WIN32
	#error Can only compile under Microsoft Windows!
#endif // _WIN32

//
// Includes
//

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include <strsafe.h>

// Direct3D includes
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>


#define TITLE "2D Window test"


typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;
typedef unsigned int uint;


int Message(uint type,const char *msg,...);

#define MAX_HEIGHT 4096
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define SGN(x) ((x) < 0 ? -1 : (x) > 0 ? 1 : 0)

#define CLAMP_ANGLE(x)							\
{												\
	if(x < 0) while(x < 0) x += 360;			\
	else if(x >= 360) while(x > 360) x -= 360;	\
}


extern POINT g_ptMouse;
extern bool g_bLmouseButt;

#endif // _MAIN_H_ 

