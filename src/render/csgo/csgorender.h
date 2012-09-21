#pragma once

#include "../../required.h"
#include "../render.h"
#include "../../util/log.h"
#include "../../framework/csgo/sdk.h"
#include "csgofont.h"

namespace ion
{
	class csgorender : public render
	{
	public:

		csgorender(SurfaceV30::ISurface* surface) : m_surface(surface)
		{
		}
		//text functions
		virtual size renderText(int flags, const font *fnt, const point& p, const color& col, const boost::format& fmt)
		{
			return renderText(flags, fnt, p, col, str(fmt));
		}
		virtual size renderText(int flags, const font *fnt, const point& p, const color& col, const std::string &fmt)
		{
			auto f = reinterpret_cast<const csgofont*>(fnt);
			wchar_t buf[512];
			size_t cvt;
			mbstowcs_s(&cvt, buf, strlen(fmt.c_str())+1, fmt.c_str(), _TRUNCATE);
			m_surface->DrawSetTextColor(col.R, col.G, col.B, col.A);
			m_surface->DrawSetTextFont(f->fnt);
			m_surface->DrawSetTextPos(p.getX(), p.getY());
			m_surface->DrawPrintText(buf, fmt.length());
			size ret;
			m_surface->GetTextSize(f->fnt, buf,  ret.m_width, ret.m_height);
			return ret;
		}

		virtual size measureText(int flags, const font *fnt, const point& p, const boost::format& fmt) {return size();}
		virtual size measureText(int flags, const font *fnt, const point& p, const std::string& fmt)
		{
			auto f = reinterpret_cast<const csgofont*>(fnt);
			wchar_t buf[512];
			size_t cvt;
			mbstowcs_s(&cvt, buf, strlen(fmt.c_str())+1, fmt.c_str(), _TRUNCATE);
			size ret;
			m_surface->GetTextSize(f->fnt, buf,  ret.m_width, ret.m_height);
			return ret;
		}

		virtual font* createFont(const std::string& name, int size, int flags, int weight)
		{
			csgofont* f = new csgofont();
			f->setSurface(m_surface);
			f->setName(name);
			f->setSize(size);

			int realFlags = 0;
			if (flags & Outline) realFlags |= csgofont::FONTFLAG_OUTLINE; 
			if (flags & Underline) realFlags |= csgofont::FONTFLAG_UNDERLINE;
			if (flags & Italic) realFlags |= csgofont::FONTFLAG_ITALIC;


			f->setFlags(flags);
			f->setWeight(weight);
			f->create();
			return (font*)f;
		}

		//rect
		virtual void fillRect(const rect& r, const color& c)
		{
			m_surface->DrawSetColor(c.R, c.G, c.B, c.A);
			m_surface->DrawFilledRect(r.getX(), r.getY(), r.getX() + r.getW(), r.getY() + r.getH());
		}
		virtual void outlineRect(const rect& r, const color& c)
		{
			m_surface->DrawSetColor(c.R, c.G, c.B, c.A);
			m_surface->DrawOutlinedRect(r.getX(), r.getY(), r.getX() + r.getW(), r.getY() + r.getH());
		}
		virtual void fillGradient(const rect& r, const color& top, const color& bottom)
		{

		}

		//line
		virtual void renderLine(const point& begin, const point& end, const color& col)
		{
			m_surface->DrawSetColor(col.R, col.G, col.B, col.A);
			m_surface->DrawLine(begin.getX(), begin.getY(), end.getX(), end.getY());
		}

		virtual ~csgorender() {}

		SurfaceV30::ISurface* m_surface;
	};
}