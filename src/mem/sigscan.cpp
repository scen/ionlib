#include "sigscan.h"

namespace ion
{
	DWORD sigscan::find()
	{
		if (!m_sig.getPattern() || !m_sig.getMask()) return 0;
		for (DWORD i = m_start; i < m_start + m_len; i++)
		{
			bool flag = true;
			for (DWORD j = 0; j < strlen(m_sig.getMask()); j++)
			{
				if (m_sig.getMask()[j] == 'x' && *(BYTE*)(i + j) != m_sig.getPattern()[j])
				{
					flag = false;
					break;
				}
			}
			if (flag)
				return i;
		}
		return NULL;
	}
}