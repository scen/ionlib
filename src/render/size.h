#pragma once

#include "../required.h"

namespace ion
{
	class size
	{
	public:
		size() : m_width(0), m_height(0) {}
		size(int w, int h) : m_width(w), m_height(h) {}

		bool operator==(const size& other) const
		{
			return getWidth() == other.getWidth() && getHeight() == other.getHeight();
		}
		bool operator!=(const size& other) const
		{
			return !(*this == other);
		}


		int getHeight() const { return m_height; }
		void setHeight(int val) { m_height = val; }
		int getWidth() const { return m_width; }
		void setWidth(int val) { m_width = val; }

		int m_width, m_height;


	};
}