#ifndef _BUFFER_H_
#define _BUFFER_H_

//
// Classes
//

class Buffer
{
private:

	HBITMAP		bufBmp,
				defBmp;
	
	HDC			bufDC,
				dibDC;
	
	void		*bits;
	byte		*bbits;
	
	dword		width,
				height;
	long		pitch;
	byte		bpp;
	

public:

	Buffer(void);

	bool Init(HDC dc,dword w,dword h);
	void Deinit(void);

	void* GetBits(void);

	dword GetWidth(void);
	dword GetHeight(void);
	long GetPitch(void);
	byte GetBPP(void);
	
	HBITMAP GetDIB(void);
	HDC GetBufDC(void);

	bool IsBuffer(void);
	//--- rutine desenare ---
	long	offs[MAX_HEIGHT];				// Buffer's vertical offsets
	void	RecalcOffs(void);

	void clearBuffer();

	void putPixel(int x, int y, byte r, byte g, byte b);
	void line(int x1,int y1,int x2,int y2, byte r, byte g, byte b);
	void drawText(int x1, int y1, char* textul, DWORD color);
	void drawEllipse(int x1, int y1, int rh, int rv, byte r, byte g, byte b);
};

#endif
