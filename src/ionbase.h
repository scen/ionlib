#pragma once
//inherit this class to create your hack

#include "required.h"
#include "luamgr/luamgr.h"
#include "luamgr/global_lua.h"
#include "util/sigdb.h"

namespace ion
{
	class ionbase
	{
	public:
		virtual ~ionbase() {}

		void reloadProject()
		{
			FILE * f = fopen(projFile.c_str(), "rb");
			if (!f)
			{
				dbglogn("project file cannot be opened");
				return;
			}

			//find base path for project
			char dir[MAX_PATH] = {0};
			char drive[MAX_PATH] = {0};
			_splitpath(projFile.c_str(), drive, dir, 0, 0); 
			basepath = std::string(drive) + std::string(dir);

			fseek(f, 0, FILE_END);
			auto sz = ftell(f);
			rewind(f);
			char* data = new char[sz + 1];
			memset(data, 0, sz + 1);
			fread(data, 1, sz, f);
			fclose(f);
			std::string contents(data);
			delete[] data;
			Json::Value root;
			Json::Reader reader;
			bool flag = reader.parse(contents, root);
			if (!flag)
			{
				dbglogn("Failed to parse project file" << std::endl << reader.getFormatedErrorMessages());
				return;
			}

			const Json::Value name = root["name"];
			if (name == Json::Value::null)
			{
				dbglogn("missing name parameter");
				return;
			}
			projName = name.asString();

			sigdbFile = root.get("sigdb", "").asString();
			parseSigs();


			lua.reloadProject(root, basepath);

		}

		void parseSigs()
		{
			if (sigdbFile.empty()) return;
			sigs._modules.clear();
			sigs._entry.clear();
			FILE * f = fopen((basepath+sigdbFile).c_str(), "rb");
			if (!f)
			{
				dbglogn("sigdb file cannot be opened");
				return;
			}

			fseek(f, 0, FILE_END);
			auto sz = ftell(f);
			rewind(f);
			char* data = new char[sz + 1];
			memset(data, 0, sz + 1);
			fread(data, 1, sz, f);
			fclose(f);
			std::string contents(data);
			delete[] data;
			Json::Value root;
			Json::Reader reader;
			if (!reader.parse(contents, root))
			{
				dbglogn("[fatal] Failed to parse sigdb file" << std::endl << reader.getFormatedErrorMessages());
				return;
			}
			scanAll = root.get("scan_all", false).asBool();
			for (auto it = root.begin(); it != root.end(); it++)
			{
				if (it.key().asString() == "scan_all") continue;
				std::string module = (*it).get("module", "").asString();
				std::string pattern = (*it).get("pattern", "").asString();
				std::string o;
				try
				{
					o = (*it).get("offset", "").asString();
				}
				catch (...)
				{
					o = "";
				}
				UINT offset = 0;
				if (o.length() >= 2 && !o.empty() && std::string(o.cbegin(), o.cbegin()+2) == "0x")
				{
					//hex
					std::stringstream ss;
					ss << std::hex << o;
					ss >> offset;
				}
				else
				{
					offset = (UINT)(*it).get("offset", 0).asInt();
				}
				if (module.empty() || pattern.empty())
				{
					dbglogn("Invalid values in signature " << it.key().asString());
					continue;
				}
				sigs.addEntry(it.key().asString(), pattern, module, offset);
			}
			if (scanAll) sigs.scanAll();
			filewatcher.addFile(basepath+sigdbFile, this, &dispatchSigReload);
		}

		void superInit(const std::string& proj)
		{
			lua.init();
			projFile = proj;
			reloadProject();
			filewatcher.addFile(projFile, this, &dispatchProjectReload);
		}

		static void dispatchProjectReload(void* inst)
		{
			auto ibase = (ionbase*)inst;
			ibase->reloadProject();
		}

		static void dispatchSigReload(void* inst)
		{
			auto ibase = (ionbase*)inst;
			ibase->parseSigs();
		}

		ionbase(const std::string& proj)
		{
			superInit(proj);
		}

		ion::sigdb sigs;
		bool scanAll;
		std::string projFile;
		std::string projName;
		std::string sigdbFile;
		std::string basepath;
	};
}