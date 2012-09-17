#pragma once

#include "../../required.h"
#include "../render.h"
#include "../../util/log.h"
#include "../../framework/source/sdk.h"
#include "sourcefont.h"

namespace ion
{
	class sourcerender : public render
	{
	public:
		
		sourcerender(vgui::ISurface* surface) : m_surface(surface)
		{
		}
		//text functions
		virtual void renderText(int flags, const font *fnt, const point& p, const color& col, const boost::format& fmt) {}
		virtual void renderText(int flags, const font *fnt, const point& p, const color& col, const std::string &fmt)
		{
			auto f = reinterpret_cast<const sourcefont*>(fnt);
			wchar_t buf[512];
			size_t cvt;
			mbstowcs_s(&cvt, buf, strlen(fmt.c_str())+1, fmt.c_str(), _TRUNCATE);
			m_surface->DrawSetTextColor(col.R, col.G, col.B, col.A);
			m_surface->DrawSetTextFont(f->font);
			m_surface->DrawSetTextPos(p.getX(), p.getY());
			m_surface->DrawPrintText(buf, fmt.length());
		}

		virtual size measureText(int flags, const font *fnt, const point& p, const boost::format& fmt) {return size();}
		virtual size measureText(int flags, const font *fnt, const point& p, const std::string& fmt)
		{
			auto f = reinterpret_cast<const sourcefont*>(fnt);
			wchar_t buf[512];
			size_t cvt;
			mbstowcs_s(&cvt, buf, strlen(fmt.c_str())+1, fmt.c_str(), _TRUNCATE);
			size ret;
			m_surface->GetTextSize(f->font, buf,  ret.m_width, ret.m_height);
			return ret;
		}

		virtual font* createFont(const std::string& name, int size, int flags, int weight)
		{
			sourcefont* f = new sourcefont();
			f->setSurface(m_surface);
			f->setName(name);
			f->setSize(size);
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

		virtual ~sourcerender() {}

		vgui::ISurface* m_surface;
	};
}