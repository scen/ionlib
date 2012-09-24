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

			size ret;
			m_surface->GetTextSize(f->fnt, buf,  ret.m_width, ret.m_height);

			m_surface->DrawSetTextColor(col.R, col.G, col.B, col.A);
			m_surface->DrawSetTextFont(f->fnt);
			if (flags & RAlign)
			{
				m_surface->DrawSetTextPos(p.getX() - ret.getWidth(), p.getY());
			}
			else if (flags & Center)
			{
				m_surface->DrawSetTextPos(p.getX() - (ret.getWidth() >> 1), p.getY());
			}
			else
			{
				m_surface->DrawSetTextPos(p.getX(), p.getY());
			}
			m_surface->DrawPrintText(buf, fmt.length());
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

		void renderCornerRect(rect& r, color& col)
		{
			float h = (float)r.getH() * 0.25f;
			float w = (float)r.getW() * 0.25f;
			auto& black = color::black();
			renderLine(point(r.m_x, r.m_y), point(r.m_x + w, r.m_y), col);
			renderLine(point(r.m_x + 1, r.m_y + 1), point(r.m_x + w, r.m_y + 1), black);
			renderLine(point(r.m_x, r.m_y), point(r.m_x, r.m_y + h), col);
			renderLine(point(r.m_x + 1, r.m_y + 1), point(r.m_x + 1, r.m_y + h), black);
			renderLine(point(r.m_x - 1, r.m_y - 1), point(r.m_x + w + 1, r.m_y - 1), black);
			renderLine(point(r.m_x - 1, r.m_y - 1), point(r.m_x - 1, r.m_y + h + 1), black);
			renderLine(point(r.m_x + w + 1, r.m_y - 1), point(r.m_x + w + 1, r.m_y + 1), black);
			renderLine(point(r.m_x - 1, r.m_y + h + 1), point(r.m_x + 1, r.m_y + h + 1), black);

			renderLine(point(r.m_x + r.m_w - w, r.m_y), point(r.m_x + r.m_w, r.m_y), col);
			renderLine(point(r.m_x + r.m_w, r.m_y), point(r.m_x + r.m_w, r.m_y + h), col);
			renderLine(point(r.m_x + r.m_w - 1, r.m_y + 1), point(r.m_x + r.m_w - 1, r.m_y + h), black);
			renderLine(point(r.m_x + r.m_w - w, r.m_y + 1), point(r.m_x + r.m_w - 1, r.m_y + 1), black);
			renderLine(point(r.m_x + r.m_w - w, r.m_y - 1), point(r.m_x + r.m_w + 1, r.m_y - 1), black);
			renderLine(point(r.m_x + r.m_w + 1, r.m_y), point(r.m_x + r.m_w + 1, r.m_y + h), black);
			renderLine(point(r.m_x + r.m_w - w - 1, r.m_y - 1), point(r.m_x + r.m_w - w - 1, r.m_y + 1), black);
			renderLine(point(r.m_x + r.m_w - 1, r.m_y + h + 1), point(r.m_x + r.m_w + 1, r.m_y + h + 1), black);

			renderLine(point(r.m_x, r.m_y + r.m_h - h), point(r.m_x, r.m_y + r.m_h), col);
			renderLine(point(r.m_x, r.m_y + r.m_h), point(r.m_x + w, r.m_y + r.m_h), col);
			renderLine(point(r.m_x + 1, r.m_y + r.m_h - h), point(r.m_x + 1, r.m_y + r.m_h - 1), black);
			renderLine(point(r.m_x + 2, r.m_y + r.m_h - 1), point(r.m_x + w, r.m_y + r.m_h - 1), black);
			renderLine(point(r.m_x, r.m_y + r.m_h + 1), point(r.m_x + w, r.m_y + r.m_h + 1), black);
			renderLine(point(r.m_x - 1, r.m_y + r.m_h - h), point(r.m_x - 1, r.m_y + r.m_h + 1), black);
			renderLine(point(r.m_x - 1, r.m_y + r.m_h - h - 1), point(r.m_x + 1, r.m_y + r.m_h - h - 1), black);
			renderLine(point(r.m_x + w + 1, r.m_y + r.m_h + 1), point(r.m_x + w + 1, r.m_y + r.m_h - 1), black);

			renderLine(point(r.m_x + r.m_w - w, r.m_y + r.m_h), point(r.m_x + r.m_w, r.m_y + r.m_h), col);
			renderLine(point(r.m_x + r.m_w, r.m_y + r.m_h - h), point(r.m_x + r.m_w, r.m_y + r.m_h), col);
			renderLine(point(r.m_x + r.m_w - 1, r.m_y + r.m_h - 1), point(r.m_x + r.m_w - 1, r.m_y + r.m_h - h), black);
			renderLine(point(r.m_x + r.m_w - w , r.m_y + r.m_h - 1), point(r.m_x + r.m_w - 1, r.m_y + r.m_h - 1), black);
			renderLine(point(r.m_x + r.m_w - w - 1, r.m_y + r.m_h + 1), point(r.m_x + r.m_w - w - 1, r.m_y + r.m_h - 1), black);
			renderLine(point(r.m_x + r.m_w - 1, r.m_y + r.m_h - h - 1), point(r.m_x + r.m_w + 1, r.m_y + r.m_h - h - 1), black);
			renderLine(point(r.m_x + r.m_w - w, r.m_y + r.m_h + 1), point(r.m_x + r.m_w + 1, r.m_y + r.m_h + 1), black);
			renderLine(point(r.m_x + r.m_w + 1, r.m_y + r.m_h), point(r.m_x + r.m_w + 1, r.m_y + r.m_h - h), black);
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