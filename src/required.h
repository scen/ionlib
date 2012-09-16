#pragma once

#define _LUA_JIT //experimental

#pragma warning(disable: 4005 4996 4615 4018 4482;)

#define BEA_ENGINE_STATIC
#define NOMINMAX
#define _SECURE_SCL 0

#define JMP_SIZE 5 //0xE9

#include <cstdio>
#include <cstdlib>
#include <varargs.h>

#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <winnt.h>
#include <winternl.h>
#include <WinBase.h>
#include <Shlwapi.h>

//STL
#include <functional>
#include <algorithm>
#include <vector>
#include <memory>
#include <sstream>
#include <fstream>
#include <ostream>
#include <istream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <set>
#include <map>

//beaengine
#include "beaengine/BeaEngine.h"

//asmjit
#include "asmjit/AsmJit.h"

//boost
#include "boost/lexical_cast.hpp"
#include "boost/format.hpp"
#include "boost/foreach.hpp"

//lua
extern "C" {
#ifdef _LUA_JIT
#include "lua/luajit/luajit.h"
#include "lua/luajit/lua.h"
#include "lua/luajit/lualib.h"
#include "lua/luajit/lauxlib.h"
#else
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
#endif
}

//luabind
#include "luabind/luabind.hpp"
#include "luabind/operator.hpp"
#include "luabind/adopt_policy.hpp"

//Dropping javascript :(
//google v8
//#include "v8/v8.h"

//V8Convert
//#include "cvv8/v8-convert.hpp"
//#include "cvv8/V8Shell.hpp"

//namespace cv = cvv8;

#pragma comment(lib, "WinMM.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "bea.lib")
#ifdef _LUA_JIT
#pragma comment(lib, "luajit.lib") //compiled with lua 5.1.5
#pragma comment(lib, "libluabind515.lib")
#else
#pragma comment(lib, "lua52.lib")
#pragma comment(lib, "libluabind52.lib")
#endif

//#pragma comment(lib, "v8/v8_base.lib")
//#pragma comment(lib, "v8/preparser_lib.lib")
//#pragma comment(lib, "v8/v8_snapshot.lib")