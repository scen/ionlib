#pragma once

#include "../../required.h"
#include "../render.h"
#include "../../framework/source/sdk.h"

namespace ion
{
	class sourcefont : public font
	{
	public:
		enum EFontFlags
		{
			FONTFLAG_NONE,
			FONTFLAG_ITALIC			= 0x001,
			FONTFLAG_UNDERLINE		= 0x002,
			FONTFLAG_STRIKEOUT		= 0x004,
			FONTFLAG_SYMBOL			= 0x008,
			FONTFLAG_ANTIALIAS		= 0x010,
			FONTFLAG_GAUSSIANBLUR	= 0x020,
			FONTFLAG_ROTARY			= 0x040,
			FONTFLAG_DROPSHADOW		= 0x080,
			FONTFLAG_ADDITIVE		= 0x100,
			FONTFLAG_OUTLINE		= 0x200,
			FONTFLAG_CUSTOM			= 0x400,	// custom generated font -
		};

		void create()
		{
			font = surface->CreateFont();
			surface->SetFontGlyphSet(font, getName().c_str(), getSize(), weight, 0, 0, flags);
		}

		int weight;
		void setWeight(int w){weight = w;}
		int flags;
		void setFlags(int f) 
		{
			flags = f;
		}

		vgui::HFont font;
		vgui::HFont getFont() const {return font;}
		vgui::ISurface* surface;
		void setSurface(vgui::ISurface* s) {surface = s;}
	};
}