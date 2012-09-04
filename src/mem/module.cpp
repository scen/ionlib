#include "module.h"

namespace ion
{
	module::module(char* name)
	{
		bool flag = false;
		enumLoadedModules([&](MODULEENTRY32& me32){
			if (!strcmp(name, me32.szModule))
			{
				setStart((DWORD)me32.modBaseAddr);
				setLen((DWORD)me32.modBaseSize);
				flag = true;
			}
		});
	}

	bool module::enumLoadedModules(std::function<void(MODULEENTRY32&)> lambda)
	{
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
		if (!hSnap) return false;
		MODULEENTRY32 me32 = {0};
		me32.dwSize = sizeof MODULEENTRY32;
		if (!Module32Next(hSnap, &me32))
		{
			CloseHandle(hSnap);
			return false;
		}
		
		do 
		{
			lambda(me32);
		} while (Module32Next(hSnap, &me32));

		CloseHandle(hSnap);
		return true;
	}
}