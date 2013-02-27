#include "main.h"
#include "buffer.h"

//standard va fi cu 3 bytes per pixel
#define STD_BPP 3
//
// Classes
//

Buffer::Buffer(void)
{
	bufBmp	= NULL;
	defBmp	= NULL;

	bufDC	= NULL;
	dibDC	= NULL;
}

bool Buffer::Init(HDC dc,dword w,dword h)
{
	BITMAPINFO	*bi;
	byte		*data;

	assert(bufBmp == NULL);	
	assert(dc != NULL);

	data = new byte[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)];
	memset(data,0,sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));
 
	bi = (BITMAPINFO*)data;
	bi->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bi->bmiHeader.biWidth			= w;
	bi->bmiHeader.biHeight			= -((long)h);		// DIBs are initially flipped
	bi->bmiHeader.biBitCount		= 8 * STD_BPP;
	bi->bmiHeader.biPlanes			= 1;
	bi->bmiHeader.biCompression		= BI_RGB;
	bi->bmiHeader.biSizeImage		= 0;
	bi->bmiHeader.biXPelsPerMeter	= 0;
	bi->bmiHeader.biYPelsPerMeter	= 0;
	bi->bmiHeader.biClrUsed			= 256;
	bi->bmiHeader.biClrImportant	= 256;

	bufBmp = CreateDIBSection(dc,bi,DIB_RGB_COLORS,&bits,NULL,0);
	if(!bufBmp)
	{
		Message(MB_OK|MB_ICONERROR,"Buffer::Init - Cannot create DIB section!");
		goto error;
	}

	bufDC = CreateCompatibleDC(dc);
	if(!bufDC)
	{
		Message(MB_OK|MB_ICONERROR,"Buffer::Init - Cannot create storage DC!");
		goto error;
	}

	defBmp = (HBITMAP)SelectObject(bufDC,bufBmp);
	if(!defBmp)
	{
		Message(MB_OK|MB_ICONERROR,"Buffer::Init - Cannot select DIB on storage DC!");
		goto error;
	}
	
	delete [] data;

	dibDC	= dc;

	width	= w;
	height	= h;
	pitch	= width * 3;

	bbits = (byte*)bits;

	return true;

error:
	if(data) delete [] data;
	Deinit();
	return false;
}

void Buffer::Deinit(void)
{
	if(bufBmp)
	{
		if(bufDC)
		{
			if(defBmp)
			{
				SelectObject(bufDC,defBmp);
				defBmp = NULL;
			}

			DeleteDC(bufDC);
			bufDC = NULL;
		}

		DeleteObject(bufBmp);
		bufBmp = NULL;
	}

	bits = NULL;

	width	= 0;
	height	= 0;
	pitch	= 0;
}


void* Buffer::GetBits(void)
{
	return bits;
}

dword Buffer::GetWidth(void)
{
	return width;
}

dword Buffer::GetHeight(void)
{
	return height;
}

long Buffer::GetPitch(void)
{
	return pitch;
}

byte Buffer::GetBPP(void)
{
	return STD_BPP;
}

HBITMAP Buffer::GetDIB(void)
{
	return bufBmp;
}

HDC Buffer::GetBufDC(void)
{
	return bufDC;
}

bool Buffer::IsBuffer(void)
{
	return (bufBmp != NULL);
}


//--- rutine grafice ---
void Buffer::RecalcOffs(void)
{
	for(long i = 0; i < MAX_HEIGHT; i++)
	{
		offs[i] = i * pitch;
	}
}

void Buffer::clearBuffer()
{
	memset(bits, 0, width * height * STD_BPP);
}

void Buffer::putPixel(int x, int y, byte r, byte g, byte b)
{
	if((x<0)||(y<0)||(x>=width)||(y>=height)) return;
	byte* tbbits = &bbits[offs[y] + x * STD_BPP];
	*tbbits = b;
	tbbits++;
	*tbbits = g;
	tbbits++;
	*tbbits = r;
}

/*
 * Draws a line using the Bresenham's line algorithm
 */


void Buffer::line(int x1,int y1,int x2,int y2,byte r, byte g, byte b)
{
	int 	deltaX,deltaY,
			absDeltaX,absDeltaY,
			x,y,
			incX,incY,
			val,
			i;
	byte	*p;

	deltaX = x2 - x1;
	deltaY = y2 - y1;

	absDeltaX = ABS(deltaX);
	absDeltaY = ABS(deltaY);

	x = x1;
	y = y1;

	incX = SGN(deltaX);
	incY = SGN(deltaY);

	if(absDeltaX >= absDeltaY)
	{
		val = absDeltaY >> 1;

		for(i = 0;i < absDeltaX;i++)
		{
			val += absDeltaY;
			if(val >= absDeltaX)
			{
				val -= absDeltaX;
				y += incY;
			}
			x += incX;
			if((x >= 0 && x < width) && (y >= 0 && y < height))
			{
				byte *tbbits = &bbits[offs[y] + x * STD_BPP];
				*tbbits = b;
				tbbits++;
				*tbbits = g;
				tbbits++;
				*tbbits = r;
			}
		}
	}
	else
	{
		val = absDeltaX >> 1;

		for(i = 0;i < absDeltaY;i++)
		{
			val += absDeltaX;
			if(val >= absDeltaY)
			{
				val -= absDeltaY;
				x += incX;
			}
			y += incY;
			if(x >= 0 && x < width &&
			   y >= 0 && y < height)
			{
				byte *tbbits = &bbits[offs[y] + x * STD_BPP];
				*tbbits = b;
				tbbits++;
				*tbbits = g;
				tbbits++;
				*tbbits = r;
			}
		}
	}
}

void Buffer::drawText(int x1, int y1, char* textul, DWORD color)
{
	RECT rc;
	SetRect(&rc, 0, 0, width, height);
	SetBkMode(bufDC, TRANSPARENT);
	SetTextColor(bufDC, color);
	DrawTextA(bufDC, textul, strlen(textul), &rc, DT_TOP|DT_LEFT); 
}

void Buffer::drawEllipse(int x1, int y1, int rh, int rv, byte r, byte g, byte b)
{
	float ang = 3.1415926535f / 2.0f;
	float step = 1.0f / (rh + rv);
	while (ang >= 0.0f)
	{
		putPixel(x1 - rh * sin(ang), y1 - rv * cos(ang), r, g, b);
		putPixel(x1 + rh * sin(ang), y1 - rv * cos(ang), r, g, b);
		putPixel(x1 - rh * sin(ang), y1 + rv * cos(ang), r, g, b);
		putPixel(x1 + rh * sin(ang), y1 + rv * cos(ang), r, g, b);
		ang -= step;
	}

}
