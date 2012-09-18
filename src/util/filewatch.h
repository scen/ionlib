#pragma once

#include "../required.h"
#include "log.h"

#define filewatcher ion::filewatch::get()

namespace ion
{
	typedef void(*FileWatchCallback_t)(void* const);
	class filewatch
	{
	public:
		class entry
		{
		public:
			std::string name;
			FileWatchCallback_t callback;
			SYSTEMTIME st;
			void* instance;
			entry(const std::string& fname, void* inst, FileWatchCallback_t pCallback) : name(fname), callback(pCallback), instance(inst)
			{
				memset(&st, 0, sizeof st);
			}
			entry()
			{
			}
		};

		void addFile(const std::string& name, void* instance, FileWatchCallback_t callback)
		{
			auto e = new entry(name, instance, callback);
			isFileModified(e->name, &e->st);
			_files.insert(std::pair<std::string, entry*>(name, e));
		}

		void removeFile(const std::string& path)
		{
			_files.erase(path);
		}

		void init()
		{
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&waitThread, 0, 0, 0);
		}

		static filewatch& get()
		{
			static filewatch fw;
			return fw;
		}
		CRITICAL_SECTION cs;
		std::map<std::string, entry*> _files;
		//std::vector<entry*> _files;
	private:
		filewatch()
		{
			InitializeCriticalSection(&cs);
		}

		static void waitThread(void* p)
		{
			EnterCriticalSection(&filewatcher.cs);
			while (true)
			{
				for (auto it = filewatcher._files.begin(); it != filewatcher._files.end(); it++)
				{
					auto inst = *it;
					if (filewatcher.isFileModified(inst.second->name, &inst.second->st))
						inst.second->callback(inst.second->instance);
				}
				Sleep(1000);
			}
		}

		bool isFileModified(const std::string& file, SYSTEMTIME* const psys)
		{
			WIN32_FILE_ATTRIBUTE_DATA data = {0};
			GetFileAttributesEx(file.c_str(),  GetFileExInfoStandard,
				&data);
			SYSTEMTIME st = {0};
			FileTimeToSystemTime(&data.ftLastWriteTime, &st);

			int ret =  memcmp(&st, psys, sizeof SYSTEMTIME);
			memcpy(psys, &st, sizeof st);
			return (bool)ret;
		}
	};
}