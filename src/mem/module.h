#pragma once

#include "../required.h"
#include "../util/log.h"
namespace ion
{
	class module
	{
	public:
		module(const std::string& name);
		module() {}
		~module() {}
		//static
		static bool enumLoadedModules(std::function<void(MODULEENTRY32&)> lambda);

		DWORD getLen() const { return m_len; }
		void setLen(DWORD val) { m_len = val; }
		DWORD getStart() const { return m_start; }
		void setStart(DWORD val) { m_start = val; }

	private:
		DWORD m_start, m_len;
	};
}