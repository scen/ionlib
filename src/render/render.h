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
			None = 0,
			Bold = 1,
			Italic = 2,
			Underline = 4,
			Center = 8,
			VCenter = 16,
			LAlign = 32,
			RAlign = 64,
			TAlign = 128,
			BAlign = 256,
		};
		virtual ~render() {}
		
		//text functions
		virtual void renderText(int flags, const font *fnt, const point& p, const color& col, const boost::format& fmt) PURE;
		virtual void renderText(int flags, const font *fnt, const point& p, const color& col, const std::string &fmt) PURE;

		virtual size measureText(int flags, const font *fnt, const point& p, const boost::format& fmt) PURE;
		virtual size measureText(int flags, const font *fnt, const point& p, const std::string& fmt) PURE;

		virtual font* createFont(const std::string& name, int size, int flags, int weight) PURE;
		//rect
		virtual void fillRect(const rect& r, const color& c) PURE;
		virtual void outlineRect(const rect& r, const color& c) PURE;
		virtual void fillGradient(const rect& r, const color& top, const color& bottom) PURE;
		
		//line
		virtual void renderLine(const point& begin, const point& end, const color& col) PURE;

		//virtual void renderCircle() PURE;
		//virtual void renderFade() PURE;
	};
}