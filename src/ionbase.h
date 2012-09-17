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

		Json::Value reloadProject(bool deferLoad = false)
		{
			FILE * f = fopen(projFile.c_str(), "rb");
			if (!f)
			{
				log.write(log.ERRO, "project file cannot be opened");
				return Json::Value::null;
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
				log.write(log.ERRO, format("Failed to parse project file\n%s") % reader.getFormatedErrorMessages());
				return Json::Value::null;
			}

			const Json::Value name = root["name"];
			if (name.isNull())
			{
				log.write(log.WARN, "missing name parameter");
				return name;
			}
			projName = name.asString();

			sigdbFile = root.get("sigdb", "").asString();
			parseSigs();


			if (!deferLoad) lua.reloadProject(root, basepath);
			return root;
		}

		void parseSigs()
		{
			if (sigdbFile.empty()) return;
			sigs._modules.clear();
			sigs._entry.clear();
			FILE * f = fopen((basepath+sigdbFile).c_str(), "rb");
			if (!f)
			{
				log.write(log.ERRO, "sigdb file cannot be opened");
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
				log.write(log.ERRO, format("Failed to parse sigdb file\n%s") % reader.getFormatedErrorMessages());
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
					log.write(log.WARN, format("Invalid values in signature %s\n") % it.key().asString());
					continue;
				}
				sigs.addEntry(it.key().asString(), pattern, module, offset);
			}
			if (scanAll) sigs.scanAll();
			filewatcher.addFile(basepath+sigdbFile, this, &dispatchSigReload);
		}

		Json::Value superInit(const std::string& proj)
		{
			lua.init();
			projFile = proj;
			auto root = reloadProject(true);
			return root;
		}

		void finishInit(Json::Value& root)
		{
			lua.reloadProject(root, basepath);
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

		ion::sigdb sigs;
		bool scanAll;
		std::string projFile;
		std::string projName;
		std::string sigdbFile;
		std::string basepath;
	};
}