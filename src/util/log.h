#pragma once

#include "../required.h"

//macros, this you can do something like this
//dbglog("this " << blah << " and that " << that)
#define log ion::logger::get()
#define dbglog(a) do { std::stringstream ss; ss << a; log.dbg(ss.str().c_str()); } while (false);
#define infolog(a) do { std::stringstream ss; ss << a; log.info(ss.str().c_str()); } while (false);

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

		logger& init(int flags)
		{
			if (flags & AllocateConsole)
			{
				AllocConsole();
				freopen("CONOUT$", "wb", stdout);
			}
			m_fp = 0;
			m_flags = flags;
			return *this;
		}

		void dbg(char* fmt, ...)
		{
#ifndef _DBG
			va_list args;
			char buf[1024] = {0};
			char tbuf[24] = {0};
			SYSTEMTIME st;

			va_start(args, fmt);
			vsprintf(buf, fmt, args);
			va_end(args);

			GetLocalTime(&st);
			sprintf_s(tbuf, 24, "[%02d:%02d:%02d]: ", st.wHour, st.wMinute, st.wSecond);

			if (m_flags & LogToConsole)
			{
				printf("%s%s", tbuf, buf);
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
			char buf[1024] = {0};
			char tbuf[24] = {0};
			SYSTEMTIME st;

			va_start(args, fmt);
			vsprintf(buf, fmt, args);
			va_end(args);

			GetLocalTime(&st);
			sprintf_s(tbuf, 24, "[%02d:%02d:%02d]: ", st.wHour, st.wMinute, st.wSecond);

			if (m_flags & LogToConsole)
			{
				printf("%s%s", tbuf, buf);
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
		logger() {}
		~logger() {}
		logger& operator=(logger const&) {}
	};
}