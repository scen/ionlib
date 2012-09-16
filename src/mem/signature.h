#pragma once

#include "../util/log.h"

namespace ion
{
	class signature
	{
	public:
		signature(std::string pattern, std::string mask) : m_pattern(pattern), m_mask(mask) {}
		signature() {}
		signature(std::string idapattern) 
		{
			static std::string wildcard("?");
			idapattern.erase(std::remove(idapattern.begin(), idapattern.end(), ' '), idapattern.end());
			for (auto it = idapattern.cbegin(); it != idapattern.cend();)
			{
				if (*it == '?')
				{
					m_pattern += '\0';
					m_mask += '?';
					it++;
					continue;
				}
				std::string cur(it, it + 2);

				std::stringstream converter(cur);
				USHORT current = 0;
                converter >> std::hex >> current >> std::dec;
                char real_current = static_cast<char>(current);
                m_pattern += real_current;
                m_mask += 'x';
				it += 2;
			}
		}
		~signature() {}
		std::string getPattern() const { return m_pattern; }
		void setPattern(std::string val) { m_pattern = val; }

		std::string getMask() const { return m_mask; }
		void setMask(std::string val) { m_mask = val; }

	private:
		std::string m_pattern;
		std::string m_mask;
		
	};
}