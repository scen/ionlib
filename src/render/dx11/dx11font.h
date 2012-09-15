#pragma once

#include "../../required.h"
#include "../render.h"

#include "extra/FW1FontWrapper.h"

#ifndef MSVC_10
#pragma comment(lib, "FW1FontWrapper.lib")
#else
#pragma comment(lib, "FW1FontWrapper.lib")
#endif

namespace ion
{
	class dx11font : public font
	{
	public:

		IFW1FontWrapper* m_fontWrapper;
		IFW1FontWrapper* getFontWrapper() const { return m_fontWrapper; }
		void setFontWrapper(IFW1FontWrapper* val) { m_fontWrapper = val; }
	};
}