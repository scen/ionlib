#pragma once

#include "../required.h"
#include <d3d11.h>
#include <D3DX10math.h>
#include <D3DX11.h>
namespace ion
{
	class color
	{
	public:
		color() : ARGB(0), d3dxcolor(D3DXCOLOR(0U)) {}
		color(ULONG argb) : ARGB(argb), d3dxcolor(D3DXCOLOR(argb)) {}
		color(UINT r, UINT g, UINT b, UINT a = 255) : A(a), R(r & 0xFF), G(g & 0xFF), B(b & 0xFF), d3dxcolor(D3DXCOLOR((float)r/255.f, (float)g/255.f, (float)b/255.f, (float)a/255.f)) {}

		UINT toABGR() const
		{
			return R | (G << 8) | ( B << 16) | (A << 24);
		}

		UINT toRGBA() const
		{
			return A | (B << 8) | (G << 16) | (R << 24);
		}

		D3DXCOLOR toD3DXCOLOR() const
		{
			return d3dxcolor;
		}

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

		D3DXCOLOR d3dxcolor;
	};
}