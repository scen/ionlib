#pragma once

#include "../required.h"
#include "jsctx.h"

namespace ion
{
	class jsmgr
	{
	public:
		struct jsctx_wrapper
		{
			jsctx* ctx;
			std::string fn;
			std::string dir;
			HANDLE hnd;
			SYSTEMTIME st;
			FILE* fin;
			void reload()
			{
				delete ctx;
				reinit();
			}
			void reinit()
			{
				char path[MAX_PATH] = {0};
				strcpy(path, fn.c_str());
				PathRemoveFileSpec(path);
				dir = std::string(path);
				fin = fopen(fn.c_str(), "rb");
				if (!fin)
				{
					dbglogn("Could not open file");
				}
				fseek(fin, 0, FILE_END);
				auto size = ftell(fin);
				rewind(fin);
				char *buf = (char*)malloc(size + 1); memset(buf, 0, size + 1);
				buf[size] = 0;
				fread_s(buf, size, 1, size, fin);
				fclose(fin);
				std::string str(buf);
				delete [] buf;
				ctx = new jsctx(str);
			}
			jsctx_wrapper(
				const std::string& mfn) : fn(mfn)
			{
				reinit();
			}
		};
		jsmgr() :
			locker(),
			contexts()
		{
			dbglogn("whats up");
			InitializeCriticalSection(&criticalSection);
		}

		void addScript(const std::string& fn, bool monitor);
		void startWaitForRefresh();
	private:
		static void waitThread(void* pInst);

		bool isFileModified(const std::string& file, SYSTEMTIME* pOld,
			SYSTEMTIME* pNew)
		{
			WIN32_FILE_ATTRIBUTE_DATA data = {0};
			GetFileAttributesEx(file.c_str(),  GetFileExInfoStandard,
				&data);
			SYSTEMTIME st = {0};
			FileTimeToSystemTime(&data.ftLastWriteTime, &st);

			int ret =  memcmp(&st, pOld, sizeof SYSTEMTIME);

			memcpy(pNew, &st, sizeof SYSTEMTIME);
			return (bool)ret;
		}

		v8::Locker locker;
		CRITICAL_SECTION criticalSection;
		std::vector<jsctx_wrapper*> contexts;
	};
}