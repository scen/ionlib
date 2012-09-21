#pragma once

#include "../required.h"

//Two classes, logger
//thanks to http://facepunch.com/showthread.php?t=1209342&p=37692735&viewfull=1#post37692735
//and https://github.com/naelstrof/Astrostruct/blob/master/src/Log.cpp
//for color inspiration (didn't know how to set color)

//#define _NO_LOG

#define log ion::logger::get()

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
		enum EColors
		{
			BLACK = 0,
			BLUE,
			GREEN,
			CYAN,
			RED,
			MAGENTA,
			BROWN,
			LIGHTGRAY,
			DARKGRAY,
			LIGHTBLUE,
			LIGHTGREEN,
			LIGHTCYAN,
			LIGHTRED,
			LIGHTMAGENTA,
			YELLOW,
			WHITE
		};

		enum ELogLevel
		{
			ERRO = 0,
			WARN,
			INFO,
			VERB
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
				if (!m_fp)
				{
					write(ERRO, "Failed to open log file\n", "logger");
				}
			}
		}

		logger& init(int flags, int width = 160, int height = 50)
		{
			if (flags & AllocateConsole)
			{
				AllocConsole();
			}
			console = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTitle("ionLOG");
			char buf[256] = {0};
			sprintf_s(buf, "mode %d,%d", width, height);
			std::system(buf);
			freopen("CONOUT$", "wb", stdout);
			/*hnd = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 
			0, 0, CONSOLE_TEXTMODE_BUFFER, 0);
			SetConsoleActiveScreenBuffer(hnd);
			SMALL_RECT sz = {0, 0, width - 1, height - 1};
			SetConsoleWindowInfo(hnd, TRUE, &sz);
			HWND hwnd = GetConsoleWindow();
			SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 1024, 700, SWP_NOMOVE); 
			SetStdHandle(STD_OUTPUT_HANDLE, hnd);
			COORD c = {width, height};
			SetConsoleScreenBufferSize(hnd, c);*/
			freopen("CONOUT$", "wb", stdout);
			m_fp = 0;
			m_flags = flags;
			return *this;
		}

		void setColor(int col)
		{
			SetConsoleTextAttribute(console, (WORD)col);
		}

		void write(int level,  const std::string& text, std::string system = "")
		{
#ifndef _NO_LOG
			write(level, boost::format(text), system);
#endif
		}

		void write(int level,  boost::format fmt, std::string system = "")
		{
#ifndef _NO_LOG
			switch (level)
			{
			case ERRO: setColor(RED); break;
			case WARN: setColor(BROWN); break;
			case INFO: setColor(LIGHTBLUE); break;
			default: setColor(DARKGRAY); break;
			}
			std::string fin = "";
			switch (level)
			{
			case ERRO: fin.append("[ERRO]: "); break;
			case WARN: fin.append("[WARN]: "); break;
			case INFO: fin.append("[INFO]: "); break;
			default: fin.append("[VERB]: "); break;
			}
			SYSTEMTIME st;
			GetLocalTime(&st);
			std::stringstream ss;
			ss << boost::str(boost::format("[%02d:%02d:%02d]") % st.wHour % st.wMinute % st.wSecond) << fin;

			if (m_flags & LogToConsole)
			{
				std::cout << ss.str();
			}

			setColor(WHITE);

			if (m_flags & LogToConsole)
			{
				std::cout << boost::str(fmt);
			}

			ss << boost::str(fmt);

			if ((m_flags & LogToFile) && m_fp)
			{
				fprintf(m_fp, ss.str().c_str());
				fflush(m_fp);
			}
			if (level == ERRO)
			{
				std::cout.flush();
				if ((m_flags & LogToFile) && m_fp)
					fflush(m_fp);
			}
#endif
		}

		void raw(const std::string& r)
		{
#ifndef _NO_LOG
			if (m_flags & LogToConsole)
			{
				std::cout << r;
			}
			if ((m_flags & LogToFile) && m_fp)
			{
				fprintf(m_fp, r.c_str());
			}
#endif
		}
	private:
		int m_flags;
		FILE* m_fp;
		HANDLE console;
		HANDLE hnd;
		logger() {}
		~logger() {}
		logger& operator=(logger const&) {}
	};

}