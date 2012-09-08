#include "jsmgr.h"

namespace ion
{

	void jsmgr::addScript(const std::string& fn, bool monitor)
	{
		EnterCriticalSection(&criticalSection);
		contexts.push_back(new jsctx_wrapper(fn));
		jsctx_wrapper* cur = contexts.back();
		LeaveCriticalSection(&criticalSection);

	}
	void jsmgr::startWaitForRefresh()
	{
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&jsmgr::waitThread, this, 0, 0);
	}

	void jsmgr::waitThread(void* pInst)
	{
		auto inst = (jsmgr*)pInst;
		//never leave the critical section
		EnterCriticalSection(&inst->criticalSection);
		BOOST_FOREACH(jsctx_wrapper* w, inst->contexts)
		{
			//set first ones
			inst->isFileModified(w->fn, &w->st, &w->st);
		}
		while (true)
		{
			BOOST_FOREACH(jsctx_wrapper* w, inst->contexts)
			{
				if (inst->isFileModified(w->fn, &w->st, &w->st))
				{
					infologn("Reloading script " << w->fn);
					w->reload();
				}
			}
			Sleep(500);
		}
		return;
		std::set<std::string> strs;
		BOOST_FOREACH(jsctx_wrapper* w, inst->contexts)
		{
			strs.insert(w->dir);
		}
		dbglogn("Handle count " << strs.size());
		HANDLE* handles = new HANDLE[strs.size()];
		int i = 0;
		BOOST_FOREACH(std::string s, strs)
		{
			handles[i] = FindFirstChangeNotification(s.c_str(), FALSE, FILE_NOTIFY_CHANGE_SIZE);
			if (handles[i] == NULL || handles[i] == INVALID_HANDLE_VALUE)
			{
				dbglogn("Error occured. FindFirstChangeNotification");
			}
			i++;
		}
		DWORD dwWaitStatus;
		while (true)
		{
			dbglogn("waiting");
			dwWaitStatus = WaitForMultipleObjects(i, handles, FALSE, INFINITE);
			if (dwWaitStatus < WAIT_OBJECT_0 || dwWaitStatus > WAIT_OBJECT_0 + (i - 1))
			{
				dbglogn("Out of range WaitForMultipleObjects");
				continue;
			}
			DWORD idx = dwWaitStatus - WAIT_OBJECT_0;
			if (FindNextChangeNotification(handles[idx]) == FALSE)
			{
				dbglogn("ERROR FindNextChangeNotification");
			}
			dbglogn("Dir changed " << inst->contexts[idx]->dir);
			dbglog(std::endl);
		}
	}

}