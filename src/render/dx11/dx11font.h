#pragma once

#include "../../required.h"
#include "../render.h"

#include "extra/FW1FontWrapper.h"

#pragma comment(lib, "FW1FontWrapper.lib")

namespace ion
{
	class dx11font : public font
	{
	public:

		int m_size;
		int getSize() const { return m_size; }
		void setSize(int val) { m_size = val; }
		IFW1FontWrapper* m_fontWrapper;
		IFW1FontWrapper* getFontWrapper() const { return m_fontWrapper; }
		void setFontWrapper(IFW1FontWrapper* val) { m_fontWrapper = val; }
	};
}