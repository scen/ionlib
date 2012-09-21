#pragma once

#include "../required.h"
#include "../util/log.h"
#include "../util/filewatch.h"

//files to expose
#include "../util/log_lua.h"
#include "../render/render.h"

#define lua ion::luamgr::get()

namespace ion
{
static char const luaStr[] = "C = ffi.C\n"
"util = util or {}\n"
"function util.makeptr(ptr, offset, type, deref)\n"
"	deref = deref or false\n"
"	local p = tonumber(ffi.cast('int', ptr))\n"
"	p = p + offset\n"
"	if deref then\n"
"		return ffi.cast(type, p)[0]\n"
"	else\n"
"		return ffi.cast(type, p)\n"
"	end\n"
"end\n"
;
	class luamgr
	{
	public:
		
		struct luascript
		{
			std::string file;
			FILE * f;
			SYSTEMTIME st;
			void reload(bool updated =  false)
			{
				f = fopen(file.c_str(), "rb");
				if (f)
				{
					fseek(f, 0, FILE_END);
					auto sz = ftell(f);
					rewind(f);
					char* data = new char[sz + 1];
					memset(data, 0, sz + 1);
					fread(data, 1, sz, f);
					fclose(f);
					std::string contents(data);
					delete[] data;
					//lua.execString(contents);
					lua.execFile(file);
					if (!updated) filewatcher.addFile(file, this, &luamgr::dispatchFileReload);
				}
				else
				{
					log.write(log.WARN, "fopen error. file not found?\n");
				}
			}
			luascript(const std::string& fn) : file(fn)
			{
				reload();
			}

		};
		struct luaproject
		{
			std::vector<luascript*> files;
		};
		static luamgr& get()
		{
			static luamgr l;
			return l;
		}

		static int errorHandler(lua_State * ls)
		{
#ifndef _NO_LOG
			lua_Debug d = {0};
			std::stringstream error;
			error << "Lua Runtime/Memory error:" << std::endl; 
			error << "************************************************" << std::endl;
			error << lua_tostring(ls, -1) << std::endl;
			error << "Stack trace:" << std::endl;
			int depth = 0;
			while (lua_getstack(ls, depth, &d))
			{
				int status = lua_getinfo(ls, "Sln", &d);
				error << "[" << depth << "] ";
				error << d.short_src << "(" << d.currentline << "): ";
				error << d.what << " " << (d.name ? d.name : "?");
				error << std::endl;
				depth++;
			}
			error << "************************************************" << std::endl;
			log.write(log.ERRO, error.str());
			//lua_pop(ls, 1); //Pop old error string
			//lua_pushstring(ls, error.str().c_str()); //push new one in
#endif
			return 1;
		}


		int execString(const std::string& str)
		{
			lua_pushcfunction(L, &errorHandler);
			luaL_loadstring(L, str.c_str());
			int ret = lua_pcall(L, 0, LUA_MULTRET, -2);
			lua_pop(L, 1);
			return ret;
		}

		int execFile(const std::string& fn)
		{
			lua_pushcfunction(L, &errorHandler);
			luaL_loadfile(L, fn.c_str());
			int ret = lua_pcall(L, 0, LUA_MULTRET, -2);
			lua_pop(L, 1);
			return ret;
		}

		void reloadProject(Json::Value& root, std::string &basepath)
		{
			curProj.files.clear();

			const Json::Value files = root["files"];
			if (files == Json::Value::null)
			{
				log.write(log.WARN, "missing files parameter\n");
			}
			for (int i = 0; i < files.size(); i++)
			{
				if (files[i]["path"] == Json::Value::null)
				{
					log.write(log.ERRO, "missing file.path\n");
					return;
				}
				auto path = files[i]["path"];
				log.write(log.INFO, "Project: Loading " + path.asString() + "\n");
				lua.addScript(basepath + path.asString());
			}

		}

		void setDrawInstance(ion::render* inst)
		{
			setGlobalVariable<ion::render>("draw", inst);
		}

		template <class T>
		void setGlobalVariable(const std::string& name, T* inst)
		{
			luabind::globals(L)[name] = inst;
		}


		//see http://www.rasterbar.com/products/luabind/docs.html#splitting-up-the-registration
		void registerScope(luabind::scope s, const char* namespac = 0)
		{
			luabind::module(L, namespac)
				[
					s
				];
		}

		template <class T>
		void addToGlobalClass(const std::string& name, T* ptr)
		{

		}

		int call(const std::string& evt)
		{
			auto &funcs = _hooks[evt];
			for (auto it = funcs.begin(); it != funcs.end(); it++)
			{
				auto &func = it->second.second;
				if (it->second.first && func.is_valid())
				{
					try
					{
						luabind::call_function<void>(func);
					}
					catch (...)
					{
						log.write(log.ERRO, "Caught exception in luabind::call\n");
						it->second.first = false;
						funcs.erase(it);
						continue;
					}
				}
				else if (it->second.first) //should be a good function, probably error
				{
					log.write(log.WARN, "Invalid function call\n");
					funcs.erase(it);
					continue;
				}
			}
			return 0;
		}

		void addScript(const std::string& s)
		{
			EnterCriticalSection(&cs);
			curProj.files.push_back(new luascript(s));
			LeaveCriticalSection(&cs);
		}

		void init()
		{
			if (!m_init) m_init = 1;
			else
			{
				return; //already initialized
			}
			EnterCriticalSection(&cs);
			L = luaL_newstate();
			luaL_openlibs(L); //load standard library

			luabind::open(L);

			luabind::set_pcall_callback(&errorHandler);

			//load FFI
			execString("ffi = require('ffi')");

			//bind some basic memory operations
			execString(luaStr);

			//bind logging class
			luabind::module(L)[
				luabind::class_<log_lua>("log")
					.scope[
						luabind::def("write", &log_lua::write),
							luabind::def("raw", &log_lua::raw)
					]
				.enum_("constants")
					[
						luabind::value("warn", log.WARN),
						luabind::value("erro", log.ERRO),
						luabind::value("verb", log.VERB),
						luabind::value("info", log.INFO)
					]
			];

			//bind hooking class
			luabind::module(L)[
				luabind::class_<luamgr>("events")
					.scope[
						luabind::def("register", &addHook),
							luabind::def("unregister", &removeHook),
							luabind::def("call", &callHook)
					]
			];

			//bind draw class
			luabind::module(L)[
				luabind::class_<ion::render>("render")
					.def("renderText", (size(render::*)(int, const font*, const point&, const color&, const std::string&))&render::renderText)
					.def("measureText", (size(render::*)(int, const font*, const point&, const std::string& ))&render::measureText)
					.def("createFont", &render::createFont)
					.def("fillRect", &render::fillRect)
					.def("outlineRect", &render::outlineRect)
					.def("fillGradient", &render::fillGradient)
					.def("renderLine", &render::renderLine)
					.enum_("constants")
					[
						luabind::value("none", ion::render::None),
						luabind::value("bold", ion::render::Bold),
						luabind::value("italic", ion::render::Italic),
						luabind::value("underline", ion::render::Underline),
						luabind::value("center", ion::render::Center),
						luabind::value("vcenter", ion::render::VCenter),
						luabind::value("lalign", ion::render::LAlign),
						luabind::value("ralign", ion::render::RAlign),
						luabind::value("talign", ion::render::TAlign),
						luabind::value("balign", ion::render::BAlign),
						luabind::value("outline", ion::render::Outline)
					]
			];

			//bind font
			luabind::module(L)[
				luabind::class_<ion::font>("font")
					.def_readwrite("name", &font::m_name)
					.def_readwrite("size", &font::m_size)
			];

			//bind rect
			luabind::module(L)[
				luabind::class_<ion::rect>("rect")
					.def(luabind::constructor<const point&, const size&>())
					.def_readwrite("h", &rect::m_h)
					.def_readwrite("w", &rect::m_w)
					.def_readwrite("x", &rect::m_x)
					.def_readwrite("y", &rect::m_y)
			];

			//bind point
			luabind::module(L)[
				luabind::class_<ion::point>("point")
					.def(luabind::constructor<int, int>())
					.def_readwrite("x", &point::m_x)
					.def_readwrite("y", &point::m_y)
			];

			//bind size
			luabind::module(L)[
				luabind::class_<ion::size>("size")
					.def(luabind::constructor<int, int>())
					.def_readwrite("w", &size::m_width)
					.def_readwrite("h", &size::m_height)
					.def(luabind::const_self == luabind::other<ion::size>())
			];

			//bind color
			luabind::module(L)[
				luabind::class_<ion::color>("color")
					.def(luabind::constructor<ULONG>())
					.def(luabind::constructor<UINT, UINT, UINT, UINT>())
					.def(luabind::constructor<UINT, UINT, UINT>())
					.def_readwrite("argb", &color::ARGB)
					.def_readwrite("a", &color::A)
					.def_readwrite("r", &color::R)
					.def_readwrite("g", &color::G)
					.def_readwrite("b", &color::B)
					.scope[
						luabind::def("red", &color::red),
							luabind::def("empty", &color::empty),
							luabind::def("green", &color::green),
							luabind::def("blue", &color::blue),
							luabind::def("black", &color::black),
							luabind::def("white", &color::white)
					]
			];


			LeaveCriticalSection(&cs);
		}

		static void dispatchFileReload(void* const inst)
		{
			luascript* instance = (luascript*)inst;
			instance->reload(true);
		}

		~luamgr()
		{
		}

		CRITICAL_SECTION cs;
		lua_State* L;

		luaproject curProj;

		luabind::object globalObject;

		static std::map<std::string, std::map<std::string, std::pair<bool, luabind::object>>> _hooks;

	private:
		luamgr() : m_init(0)
		{
			InitializeCriticalSection(&cs);
		}

		bool m_init;

	private:
		static void addHook(const std::string& evt, const std::string& id,
			luabind::object const &func) //expose to lua
		{
			log.write(log.INFO, "Registered event " + evt + " -> " + id + "\n");
			auto &h = _hooks[evt][id];
			h.second = func;
			h.first = true;
		}

		static void removeHook(const std::string& evt, const std::string& id)
		{
			log.write(log.INFO, "Unregistered event " + evt + "  -> " + id + "\n");
			auto &h = _hooks[evt][id];
			h.first = false;
		}

		static void callHook(const std::string& evt)
		{
			lua.call(evt);
		}
	};


	std::map<std::string, std::map<std::string, std::pair<bool, luabind::object>>> luamgr::_hooks;
}