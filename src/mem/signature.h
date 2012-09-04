#pragma once

namespace ion
{
	class signature
	{
	public:
		signature(char* pattern, char* mask) : m_pattern((PBYTE)pattern), m_mask(mask) {}
		signature(PBYTE pattern, char* mask) : m_pattern(pattern), m_mask(mask) {}

		PBYTE getPattern() const { return m_pattern; }
		void setPattern(PBYTE val) { m_pattern = val; }

		char* getMask() const { return m_mask; }
		void setMask(char* val) { m_mask = val; }

	private:
		PBYTE m_pattern;
		char* m_mask;
		
	};
}