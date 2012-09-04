#pragma once

//inherit from this

#include "../required.h"
#include "font.h"
#include "rect.h"
#include "point.h"
#include "size.h"
#include "color.h"

namespace ion
{
	class render
	{
	public:
		enum EFontFlags
		{

		};
		virtual ~render() {}
		
		//text functions
		virtual void renderText(const font &fnt, const point& p, const char* fmt, ...) PURE;
		virtual void renderText(int flags, const font &fnt, const point& p, const char* fmt, ...) PURE;
		virtual void renderText(int flags, const font &fnt, int x, int y, const char* fmt, ...) PURE;

		virtual size measureText(int flags, const font &fnt, const point& p, const char* fmt, ...) PURE;
		void lol()
		{
			boost::format("sup");
		}
		//rect
		virtual void renderRect(rect& r) PURE;
		virtual void renderGradient() PURE;


	};
}