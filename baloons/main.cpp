#include "main.h"
#include "buffer.h"
#include "baloons.h"

#include <d3d9.h>

//
// Constants
//

#define pi 3.14159265358

#define START_RES_X 640
#define START_RES_Y 480

//
// Macros
//

#define SIN(x) (sinTbl[(dword)x%360])
#define COS(x) (cosTbl[(dword)x%360])


//
// Variables
//

//--- variabile de mouse ---
POINT g_ptMouse;
bool g_bLmouseButt, g_bRmouseButt;


HINSTANCE	appInst = NULL;					// Application's module handle

HWND		dispWnd = NULL;					// Our render window
HDC			dispDC = NULL;					// Render window's DC

bool		isFullscr = false;				// Fullscreen flag

bool		appRunning = false;				// Is the app active?

Buffer		buffer;							// Our render buffer


dword		timeCurr,
			timeLast,	
			frmCounter,
			fps;
//timere pentru dtime la updateframe
LONGLONG			m_llQPFTicksPerSec;
LONGLONG			m_llLastElapsedTime;

float		sinTbl[360],					// Sin table
			cosTbl[360];					// Cos talbe

CBaloons	baloons;

//
// Functions
//

bool SysInit(HINSTANCE hInst);
void SysDeinit(void);

bool WindowCreate(dword w,dword h,dword bpp,bool fullscr);
void WindowDestroy(void);
LRESULT CALLBACK WindowProc(HWND wnd,uint msg,WPARAM wParam,LPARAM lParam);
bool ChangeDispSettings(dword w,dword h,dword bpp);

bool SwapBuffers(void);

void InitSinCos(void);

void UpdateFPS(void);

void UpdateFrame(double dTime);
void RenderFrame(double dTime);


void InitApp()
{
}

/* WinMain - entry point for the Windows apps */

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrevInst,
				   LPSTR cmdLine,int cmdShow)
{
	MSG msg;
	g_bLmouseButt = false;

	if(!SysInit(hInst))
		return -1;

	appRunning = true;

	buffer.RecalcOffs();

	InitSinCos();

	InitApp();

	// Initialzie the FPS counting variables

	fps			= 0;
	frmCounter	= 0;
	timeLast	= GetTickCount();

	//initialize dTime vars	pentru dTime
	LARGE_INTEGER qwTicksPerSec;
	QueryPerformanceFrequency(&qwTicksPerSec);
	LONGLONG m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;

	LARGE_INTEGER qwTicks;
	QueryPerformanceCounter(&qwTicks);
	m_llLastElapsedTime = qwTicks.QuadPart;

	while(appRunning)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			switch(msg.message)
			{
			case WM_QUIT:
				{
					appRunning = false;
				}
				break;
			case WM_MOUSEMOVE:
				{
					g_ptMouse.x = GET_X_LPARAM(msg.lParam); 
					g_ptMouse.y = GET_Y_LPARAM(msg.lParam); 
				}
				break;
			case WM_LBUTTONDOWN:
				{
					g_bLmouseButt = true;
				}
				break;
			case WM_LBUTTONUP:
				{
					g_bLmouseButt = false;
				}
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		if(buffer.IsBuffer())
		{
			// Clear buffer
			buffer.clearBuffer();
			
			LARGE_INTEGER qwTime;
			QueryPerformanceCounter(&qwTime);

			double fElapsedTime = (double) (qwTime.QuadPart - m_llLastElapsedTime) / (double) m_llQPFTicksPerSec;
			m_llLastElapsedTime = qwTime.QuadPart;

			//max 5000 fps	si min 30 fps
			if(fElapsedTime <= 0.0001f)
				fElapsedTime = 0.0001f;
			if(fElapsedTime > 0.04f)
				fElapsedTime = 0.04f;
			//Update stuff
			UpdateFrame(fElapsedTime);
			// Render stuff
			RenderFrame(fElapsedTime);
			
			// Swap buffers
			SwapBuffers();

			// Keep track of FPS
			UpdateFPS();
		}
	}

	SysDeinit();

	return 0;
}

int Message(uint type,const char *msg,...)
{
	char	message[512];
	va_list argList;

	assert(msg != NULL);

	va_start(argList,msg);
	vsprintf(message,msg,argList);
	va_end(argList);

	return MessageBox(GetFocus(),message,"MSG",type);
}

/*
 * Initialzies the system
 */

bool SysInit(HINSTANCE hInst)
{
	assert(hInst != NULL);
	
	appInst = hInst;

	bool fullscr = true;
	if(Message(MB_YESNO,"Run windowed ?") == IDYES)
		fullscr = false;

	if(!WindowCreate(START_RES_X, START_RES_Y, 16,fullscr))
		return false;
	
	if(!buffer.Init(dispDC, START_RES_X, START_RES_Y))
	{
		WindowDestroy();
		return false;
	}

	return true;
}

/*
 * Shutdowns the system
 */

void SysDeinit(void)
{
	buffer.Deinit();
	WindowDestroy();
}

/*
 * Creates our window
 */

bool WindowCreate(dword w,dword h,dword bpp,bool fullscr)
{
	WNDCLASS	wc;
	RECT		winRect;
	dword		style = WS_OVERLAPPEDWINDOW;

	assert(dispWnd == NULL);
	assert(dispDC == NULL);
	assert(appInst != NULL);

	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC)WindowProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= appInst;
	wc.hIcon			= LoadIcon(NULL,IDI_WINLOGO);
	wc.hCursor			= LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= TITLE;
	
	if(!RegisterClass(&wc))
	{
		Message(MB_OK|MB_ICONERROR,"WindowCreate - Cannot register class!");
		return false;
	}

	if(fullscr)
	{
		if(!ChangeDispSettings(w,h,bpp))
		{
			Message(MB_OK|MB_ICONEXCLAMATION,"WindowCreate - Cannot change display settings to "
											 "%dx%d %d bpp!\nProceeding in windowed mode!",w,h,bpp);
			fullscr = false;
		}
		else
		{
			while(ShowCursor(FALSE) >= 0);
			style = WS_POPUP;
		}
	
		isFullscr = fullscr;
	}

	winRect.left	= 0;
	winRect.top		= 0;
	winRect.right	= w-1;
	winRect.bottom	= h-1;

	AdjustWindowRect(&winRect,style,FALSE);

	dispWnd = CreateWindow(TITLE,
						   TITLE,
						   style,
						   0,
						   0,
						   (winRect.right - winRect.left) + 1,
						   (winRect.bottom - winRect.top) + 1,
						   NULL,
						   NULL,
						   appInst,
						   0);
	if(!dispWnd)
	{
		Message(MB_OK|MB_ICONERROR,"WindowCreate - Cannot create window!");
		WindowDestroy();
		return false;
	}

	dispDC = GetDC(dispWnd);
	if(!dispDC)
	{
		Message(MB_OK|MB_ICONERROR,"WindowCreate - Cannot get DC!");
		WindowDestroy();
		return false;
	}

	ShowWindow(dispWnd,SW_SHOWNORMAL);
	SetForegroundWindow(dispWnd);

	return true;
}

/*
 * Destroys the window
 */

void WindowDestroy(void)
{
	assert(appInst != NULL);

	if(isFullscr)
	{
		ChangeDisplaySettings(NULL,0);
		isFullscr = false;
	}

	if(dispWnd)
	{
		if(dispDC)
		{
			ReleaseDC(dispWnd,dispDC);
			dispDC = NULL;
		}

		DestroyWindow(dispWnd);
		dispWnd = NULL;
	}

	UnregisterClass(TITLE,appInst);
}

LRESULT CALLBACK WindowProc(HWND wnd,uint msg,WPARAM wParam,LPARAM lParam)
{
	long ret = 0;

	switch(msg)
	{
	case WM_SIZE:
		
		// Skip if no window or buffer

		if(dispWnd == NULL || !buffer.IsBuffer())
			break;

		// Get new width and height

		dword w,h;

		w = lParam & 0x0000FFFF;
		h = (lParam & 0xFFFF0000) >> 16;

		// Skip if zero

		if(!w || !h)
			break;

		// Destroy previous buffer and create new one

		buffer.Deinit();

		if(!buffer.Init(dispDC,(w+3)&~3,h))
		{
			appRunning = false;
		}
		else
		{
			buffer.RecalcOffs();
		}

		break;
	case WM_KEYDOWN:

		switch(wParam)
		{
		case VK_F1:
			break;
		case VK_F2:
			break;
		case VK_F3:
			break;
		case VK_LEFT:
			break;
		case VK_RIGHT:
			break;
		case VK_UP:
			break;
		case VK_DOWN:
			break;
		case VK_ESCAPE: appRunning = false;
		}
	
		break;
	case WM_CHAR:

		switch(wParam)
		{
		case 'A':
		case 'a':
			
			break;
		case 'Z':
		case 'z':

			break;
		}
		
		break;
	case WM_CLOSE: appRunning = false;
		break;
	default: ret = DefWindowProc(wnd,msg,wParam,lParam);
	}

	return ret;
}

bool ChangeDispSettings(dword w,dword h,dword bpp)
{
	DEVMODE scrSettings;

	// First check if the current display settings are the
	// ones we need
	
	EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&scrSettings);
	if(scrSettings.dmPelsWidth == w &&
	   scrSettings.dmPelsHeight == h &&
	   scrSettings.dmBitsPerPel == bpp)
	   return true;

	// Set the parameters for the new display mode

	scrSettings.dmFields		= DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
	scrSettings.dmPelsWidth		= w;
	scrSettings.dmPelsHeight	= h;
	scrSettings.dmBitsPerPel	= bpp;

	// Attemp to set the new disp mode

	if(ChangeDisplaySettings(&scrSettings,0x00000004) != DISP_CHANGE_SUCCESSFUL)
		return false;

	// Give Windows time to recover from disp change

	Sleep(1000);

	return true;
}

bool SwapBuffers(void)
{
	assert(dispDC != NULL);
	assert(buffer.GetBufDC() != NULL);

	// Get window's client rect

	RECT clRect;
	GetClientRect(dispWnd,&clRect);

	// Blit to buffer to the window

	if(BitBlt(dispDC,
			  clRect.left,
			  clRect.top,
			  (clRect.right - clRect.left) + 1, 
			  (clRect.bottom - clRect.top) + 1,
			  buffer.GetBufDC(),
			  0,
			  0,
			  SRCCOPY) == FALSE)
	{
		Message(MB_OK|MB_ICONERROR,"SwapBuffers - Cannot swap buffers!");
		return false;
	}
	
	return true;
}

/*
 * Initializes and sets the buffer's palette
 */


/*
 * Initializes the sin/cos LUTs
 */

void InitSinCos(void)
{
	double	val;
	long	i;

	for(i = 0;i < 360;i++)
	{
		val = ((double)i) * pi/180;
		sinTbl[i] = (float)sin(val);
		cosTbl[i] = (float)cos(val);
	}
}

/*
 * Keeps track of the FPS
 */

void UpdateFPS(void)
{
	frmCounter++;
	timeCurr = GetTickCount();
	if(timeCurr - timeLast >= 1000)
	{
		fps			= frmCounter;
		frmCounter	= 0;
		timeLast	= timeCurr;
	}
}


///--- AICI FACE UPDATE-urile ---
void UpdateFrame(double dTime)
{
	baloons.updateBaloons(dTime);
}

///--- AICI FACE RENDER ---
void RenderFrame(double dTime)
{
	long i;

	float	scrX,scrY;
	long	x1,y1,x2,y2;

	baloons.paintBaloons(&buffer);


	//--- draw mouse cursor ---
	if(g_bLmouseButt)
	{
		buffer.line(g_ptMouse.x - 5, g_ptMouse.y , g_ptMouse.x + 4, g_ptMouse.y , 0, 180, 0);
		buffer.line(g_ptMouse.x , g_ptMouse.y - 5, g_ptMouse.x, g_ptMouse.y + 4, 0, 180, 0);
	}
	else
	{
		buffer.line(g_ptMouse.x - 6, g_ptMouse.y , g_ptMouse.x + 5, g_ptMouse.y , 0, 255, 0);
		buffer.line(g_ptMouse.x , g_ptMouse.y - 6, g_ptMouse.x, g_ptMouse.y + 5, 0, 255, 0);
	}


///--- draw FPS ---
	char fpstxt[MAX_PATH];
	StringCchPrintfA(fpstxt, MAX_PATH, "FPS: %d dTime:%.5f", fps, dTime);
	buffer.drawText(0, 0, fpstxt, 0x00ff00ff);
}

