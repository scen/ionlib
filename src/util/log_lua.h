#pragma once

#include "../required.h"
#include "log.h"

class log_lua
{
public:
	static void write(int level, const std::string& text)
	{
		log.write(level, text, "LUA");
	}
	static void raw(const std::string& txt)
	{
		log.raw(txt.c_str());
	}
};