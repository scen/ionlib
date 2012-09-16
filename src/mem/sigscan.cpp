#include "sigscan.h"

namespace ion
{
	DWORD sigscan::find()
	{
		if (ran) return result;
		ran = true;
		for (DWORD i = m_start; i < m_start + m_len; i++)
		{
			bool flag = true;
			for (DWORD j = 0; j < m_sig.getMask().length(); j++)
			{
				if (IsBadReadPtr((BYTE*)(i + j), 1)) //Cannot be right, access violation reading it
				{
					flag = false;
					break;
				}
				if (m_sig.getMask()[j] == 'x' && *(BYTE*)(i + j) != (BYTE)m_sig.getPattern()[j])
				{
					flag = false;
					break;
				}
			}
			if (flag)
				return result = i;
		}
		return result = NULL;
	}
}