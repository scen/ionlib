#pragma once

#include "../required.h"
#include "luamgr.h"

//global class for lua

#define glua ion::global_lua::get()

namespace ion
{
	class global_lua
	{
	public:
		static global_lua& get()
		{
			static global_lua l;
			return l;
		}

		//Binds itself to the lua instance
		void init()
		{
			lua.registerScope(
				luabind::class_<global_lua>("global_lua")
					.def_readonly("game", &global_lua::game)
					.def_readonly("ingame", &global_lua::inGame)
				);
			lua.setGlobalVariable("global", this);
		}

		std::string game;
		bool inGame;

		~global_lua() {}

	private:
		global_lua()
		{

		}
	};
}