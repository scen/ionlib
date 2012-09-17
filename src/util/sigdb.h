#pragma once

#include "required.h"
#include "../mem/signature.h"
#include "../mem/sigscan.h"

namespace ion
{
	class sigdb
	{
	public:
		struct entry
		{
			std::string pat;
			module mod;
			UINT offset;
			signature s;
			sigscan ss;
			entry(const std::string& apat, module& amod, UINT of) : pat(apat), mod(amod), offset(of)
			{
				s = signature(pat);
				ss = sigscan(s, mod);
			}
			DWORD find()
			{
				return ss.find();
			}
		};
		sigdb()
		{
		}

		DWORD get(const std::string& name)
		{
			auto it = _entry.find(name);
			if (it == _entry.end())
				return 0;
			else
				return it->second.find() + it->second.offset;
		}

		DWORD operator[](const std::string& name)
		{
			return get(name);
		}

		void scanAll()
		{
			for (auto it = _entry.begin(); it != _entry.end(); it++)
			{
				it->second.find();
				log.write(log.INFO, format("%s -> 0x%X\n") % it->first % get(it->first), "sigdb");
			}
		}

		void addEntry(const std::string& name, const std::string& pat, const std::string& mod, UINT offset)
		{
			if (_modules.find(mod) == _modules.end())
			{
				_modules.insert(std::pair<std::string, module>(mod, module(mod)));
			}
			_entry.insert(std::pair<std::string, entry>(name, entry(pat, _modules[mod], offset)));
		}

		std::map<std::string, module> _modules;
		std::map<std::string, entry> _entry;
	};
}