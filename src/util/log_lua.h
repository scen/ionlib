#pragma once

#include "../required.h"

class log_lua
{
public:
	static void dbg(const std::string& text)
	{
		infologn("[lua-dbg] " << text);
	}
	static void info(const std::string& text)
	{
		infologn("[lua-info] " << text);
	}
	static void raw(const std::string& text)
	{
		log.raw((char*)text.c_str());
	}
};