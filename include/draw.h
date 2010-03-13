#ifndef DRAW_H__
#define DRAW_H__

#include <nds.h>

namespace draw
{
	template <class T>
	void swap(T& first, T& second);

	void plot(int x, int y, int colour = RGB15(31,31,31));

	void line(int x0, int y0, int x1, int y1, int colour = RGB15(31,31,31));
}

#endif // #ifndef DRAW_H__
