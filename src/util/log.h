#pragma once

#include "../required.h"

//Two classes, logger

#define _DBG 1

//macros, this you can do something like this
//dbglog("this " << blah << " and that " << that)
#define log ion::logger::get()
#ifdef _DBG
#define dbglog(a) do { char fname[_MAX_FNAME] = {0}; char extName[_MAX_EXT] = {0}; _splitpath_s(__FILE__, 0, 0, 0, 0, fname, _MAX_FNAME, extName, _MAX_EXT); std::stringstream ss; ss << fname << extName << ":" << std::dec << __LINE__ << ":" << __FUNCTION__ << "(): " << a; log.dbg((char*)ss.str().c_str()); } while (false);
#define dbglogn(a) dbglog(a << std::endl)
#else
#define dbglog(a)
#define dbglogn(a)
#endif
#define infolog(a) do { std::stringstream ss; ss << a; log.info((char*)ss.str().c_str()); } while (false);
#define infologn(a) do { std::stringstream ss; ss << a << std::endl; log.info((char*)ss.str().c_str()); } while (false);
namespace ion
{
	class logger
	{
	public:
		enum ELogFlags
		{
			LogToFile = 1,
			LogToConsole = 2,
			AllocateConsole = 4,
		};
		static logger& get()
		{
			static logger l;
			return l;
		}

		void open(const char* file)
		{
			if (m_flags & LogToFile)
			{
				m_fp = fopen(file, "wb");
			}
		}

		logger& init(int flags, int width = 160, int height = 40)
		{
			if (flags & AllocateConsole)
			{
				AllocConsole();
				console = GetStdHandle(STD_OUTPUT_HANDLE);
				//sue me
				char buf[256] = {0};
				sprintf_s(buf, "mode %d,%d", width, height);
				std::system(buf);
			}
			m_fp = 0;
			m_flags = flags;
			return *this;
		}

		void dbg(char* fmt, ...)
		{
#ifdef _DBG
			va_list args;
			char buf[32000] = {0};
			char tbuf[24] = {0};
			SYSTEMTIME st;

			va_start(args, fmt);
			vsprintf(buf, fmt, args);
			va_end(args);

			GetLocalTime(&st);
			sprintf_s(tbuf, 24, "[%02d:%02d:%02d]: ", st.wHour, st.wMinute, st.wSecond);

			if (m_flags & LogToConsole)
			{
				WriteConsoleA(console, tbuf, strlen(tbuf), NULL, NULL);
				WriteConsoleA(console, buf, strlen(buf), NULL, NULL);
			}
			if (m_fp && (m_flags & LogToFile))
			{
				fprintf(m_fp, "%s%s", tbuf, buf);
			}
#endif
		}
		void info(char* fmt, ...)
		{
			va_list args;
			char buf[32000] = {0};
			char tbuf[24] = {0};
			SYSTEMTIME st;

			va_start(args, fmt);
			vsprintf(buf, fmt, args);
			va_end(args);

			GetLocalTime(&st);
			sprintf_s(tbuf, 24, "[%02d:%02d:%02d]: ", st.wHour, st.wMinute, st.wSecond);

			if (m_flags & LogToConsole)
			{
				WriteConsoleA(console, tbuf, strlen(tbuf), NULL, NULL);
				WriteConsoleA(console, buf, strlen(buf), NULL, NULL);
			}
			if (m_fp && (m_flags & LogToFile))
			{
				fprintf(m_fp, "%s%s", tbuf, buf);
				fflush(m_fp);
			}
		}
	private:
		int m_flags;
		FILE* m_fp;
		HANDLE console;
		logger() {}
		~logger() {}
		logger& operator=(logger const&) {}
	};

}