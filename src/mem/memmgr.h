#pragma once

#include "../required.h"

namespace ion
{
	class memmgr
	{
	public:
		static bool writeBytes(PBYTE dst, std::vector<BYTE>& bytes)
		{
			DWORD dwOldProt;
			VirtualProtect(dst, bytes.size(), PAGE_READWRITE, &dwOldProt);

			for (int i = 0; i < bytes.size(); i++)
				*(dst + i) = bytes[i];

			VirtualProtect(dst, bytes.size(), dwOldProt, &dwOldProt);
			return true;
		}
		static std::vector<BYTE> readBytes(PBYTE src, int len)
		{
			std::vector<BYTE> ret;
			ret.clear();

			for (int i = 0; i < len; i++)
				ret.push_back(*(src + i));

			return ret;
		}
	};
}