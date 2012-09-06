#pragma once

#include "../required.h"

namespace ion
{
	class point
	{
	public:
		point() : m_x(0), m_y(0) {}
		point(int x, int y) : m_x(x), m_y(y) {}
		int m_x, m_y;
		int getY() const { return m_y; }
		void setY(int val) { m_y = val; }
		int getX() const { return m_x; }
		void setX(int val) { m_x = val; }
	};
}