#pragma once

#include "../required.h"
#include "point.h"
#include "size.h"

namespace ion
{
	class rect
	{
	public:
		rect() : m_x(0), m_y(0), m_w(0), m_h(0) {}
		rect(const point& pt, const size& sz) : m_x(pt.getX()), m_y(pt.getY()), m_w(sz.getWidth()), m_h(sz.getHeight()) {}
		int m_x, m_y, m_w, m_h;
		int getH() const { return m_h; }
		void setH(int val) { m_h = val; }
		int getW() const { return m_w; }
		void setW(int val) { m_w = val; }
		int getY() const { return m_y; }
		void setY(int val) { m_y = val; }
		int getX() const { return m_x; }
		void setX(int val) { m_x = val; }
	};
}