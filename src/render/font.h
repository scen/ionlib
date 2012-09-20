#pragma once

#include "../required.h"
#include "../util/log.h"
//renderers should inherit from this class

namespace ion
{
	class font
	{
	public:
		virtual ~font() {
			log.write(log.ERRO, "Destructor called!\n");
		}

		std::string m_name;
		int m_size;
		int getSize() const { return m_size; }
		void setSize(int val) { m_size = val; }
		std::string getName() const { return m_name; }
		void setName(const std::string &val) { m_name = val; }
	};
}