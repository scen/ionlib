#pragma once

#include "log.h"

namespace ion
{
	class log_js
	{
	public:
		static v8::Handle<v8::Value> dbg(const v8::Arguments& args)
		{
			if (args.Length())
			{
				std::string fin = "";
				for (int i = 0; i < args.Length(); i++)
				{
					fin += std::string(*v8::String::Utf8Value(args[i]->ToString()));
					if (i != args.Length() - 1) fin += " ";
				}
				infologn("[js-dbg] " << fin);
			}
			return v8::Undefined();
		}

		static v8::Handle<v8::Value> info(const v8::Arguments& args)
		{
			if (args.Length())
			{
				std::string fin = "";
				for (int i = 0; i < args.Length(); i++)
				{
					fin += std::string(*v8::String::Utf8Value(args[i]->ToString()));
					if (i != args.Length() - 1) fin += " ";
				}
				infologn("[js-inf] " << fin);
			}
			return v8::Undefined();
		}
	};
}