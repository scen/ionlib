#pragma once

#include "../required.h"
#include "memmgr.h"

namespace ion
{
	class detour
	{
	public:
		detour(BYTE *src, BYTE* dst) : m_src(src), m_dst(dst), m_bApplied(0) {}
		detour() {}
		~detour() {}

		bool apply();
		void remove();
		template<typename Fn> Fn tramp() const
		{
			return (Fn)m_tramp;
		}
		void jmp(void* addy, void* target);
	private:
		BYTE* m_src;
		BYTE* m_dst;
		void* m_tramp;
		bool m_bApplied;
		std::vector<BYTE> m_orig;
	};
}