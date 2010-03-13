#ifndef DRAW_CPP___
#define DRAW_CPP___

#include "draw.h"
#include <cstdlib>


template <class T>
void draw::swap(T& first, T& second)
{
	int temp = first;
	first = second;
	second = temp;
}

void draw::plot(int x, int y, int colour)
{
	if (x >= 0 && x <= SCREEN_WIDTH && y >= 0 && y <= SCREEN_HEIGHT)
    {
        VRAM_A[x + y * SCREEN_WIDTH] = colour;
    }
}

void draw::line(int x0, int y0, int x1, int y1, int colour)
{
	bool steep = abs(y1 - y0) > abs(x1 - x0);

	if (steep)
	{
		 swap(x0, y0);
		 swap(x1, y1);
	}
	if (x0 > x1)
	{
		swap(x0, x1);
		swap(y0, y1);
	}
	int deltax = x1 - x0;
	int deltay = abs(y1 - y0);
	int error = deltax / 2;
	int ystep = 0;
	int y = y0;
	if (y0 < y1)
	{
		ystep = 1;
	}
	else
	{
		ystep = -1;
	}
	for (int x = x0; x < x1; ++x)
	{
		if (steep)
		{
			plot(y,x,colour);
			plot(y+1,x,colour);
			plot(y-1,x,colour);
			plot(y,x+1,colour);
			plot(y,x-1,colour);
		}
		else
		{
			plot(x,y,colour);
			plot(x+1,y,colour);
			plot(x-1,y,colour);
			plot(x,y+1,colour);
			plot(x,y-1,colour);
		}
		error = error - deltay;

		if (error < 0)
		{
			y = y + ystep;
			error = error + deltax;
		}
	}

}

#endif // #ifndef DRAW_CPP___
