#pragma once

#include "../required.h"

namespace ion
{
	class color
	{
	public:
		color() : ARGB(0) {}
		color(ULONG argb) : ARGB(argb) {}
		color(UINT r, UINT g, UINT b, UINT a = 255) : A(a), R(r & 0xFF), G(g & 0xFF), B(b & 0xFF) {}

		//predefined colors
		static color& empty() { static color& c = color(); return c; }
		static color& red() { static color& c = color(255, 0, 0); return c; }
		static color& green() { static color& c = color(0, 255, 0); return c; }
		static color& blue() { static color& c = color(0, 0, 255); return c; }
		static color& black() { static color& c = color(0, 0, 0); return c; }
		static color& white() { static color& c = color(255, 255, 255); return c; }

		union
		{
			struct  
			{
				BYTE B, G, R, A;
			};
			ULONG ARGB;
		};
	};
}