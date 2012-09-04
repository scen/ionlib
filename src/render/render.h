#pragma once

//inherit from this

#include "../required.h"
#include "font.h"
#include "rect.h"
#include "point.h"
#include "size.h"

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
		virtual void renderText(int flags, const std::shared_ptr<font> &fnt, const point& p, const char* fmt, ...) PURE;
		virtual size measureText(int flags, const std::shared_ptr<font> &fnt, const point& p, const char* fmt, ...) PURE;

		//rect
		virtual void renderRect() PURE;
		virtual void renderGradient() PURE;


	};
}