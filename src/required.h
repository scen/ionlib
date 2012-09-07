#pragma once

#pragma warning(disable: 4005 4996 4615 4018 4482;)

#define BEA_ENGINE_STATIC
//#define BEA_USE_STDCALL
#define NOMINMAX

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

//beaengine
#include "beaengine/BeaEngine.h"

//asmjit
#include "asmjit/AsmJit.h"

//boost
#include "boost/lexical_cast.hpp"
#include "boost/format.hpp"

//google v8
#include "v8/v8.h"

//V8Convert
#include "cvv8/v8-convert.hpp"
#include "cvv8/V8Shell.hpp"

namespace cv = cvv8;

#pragma comment(lib, "WinMM.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "bea.lib")
#pragma comment(lib, "v8/v8_base.lib")
#pragma comment(lib, "v8/preparser_lib.lib")
#pragma comment(lib, "v8/v8_snapshot.lib")