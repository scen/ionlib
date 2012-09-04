#pragma once

#define BEA_ENGINE_STATIC
//#define BEA_USE_STDCALL

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

//beaengine
#include "beaengine/BeaEngine.h"

//asmjit
#include "asmjit/AsmJit.h"

#pragma comment(lib, "bea.lib")

#pragma warning(disable: 4005 4996 4615 4018 4482;)