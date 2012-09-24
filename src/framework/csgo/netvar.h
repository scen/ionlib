#pragma once

#include "../../required.h"
#include "sdk.h"

//Make sure you only initialize this once

namespace ion
{
	class netvar
	{
	public:
		netvar(IBaseClientDLL* gclient) : client(gclient)
		{
			dump();
			inst = this;
			RecvProp* p;
			auto t = findTable("DT_BasePlayer");
			ply_LifeState = findOffset(t, "m_lifeState");
			ply_Health = findOffset(t, "m_iHealth");
			ply_Flags = findOffset(t, "m_fFlags");

			t = findTable("DT_BaseCombatCharacter");
			ply_ActiveWeapon = findOffset(t, "m_hActiveWeapon");

			t = findTable("DT_BaseCombatWeapon");
			wep_Owner = findOffset(t, "m_hOwner");
			wep_Clip1 = findOffset(t, "m_iClip1");
			wep_Clip2 = findOffset(t, "m_iClip2");

			t = findTable("DT_PlantedC4");
			c4_Ticking = findOffset(t, "m_bBombTicking");
			c4_Blow = findOffset(t, "m_flC4Blow");
			c4_TimerLen = findOffset(t, "m_flTimerLength");
			c4_DefuseLen = findOffset(t, "m_flDefuseLength");
			c4_DefuseCountDown = findOffset(t, "m_flDefuseCountDown");

			t = findTable("DT_BaseEntity");
			ply_Team = findOffset(t, "m_iTeamNum");

			t = findTable("DT_BaseCombatCharacter");
			//ply_ActiveWeapon = findOffset(t, "m_hActiveWeapon");

			ply_Origin = get("DT_CSPlayer->cslocaldata->DT_CSLocalPlayerExclusive->m_vecOrigin");
			ply_EyePos = get("DT_BasePlayer->localdata->DT_LocalPlayerExclusive->m_vecViewOffset[0]");
		}
		~netvar();

#pragma region impl
		RecvTable* findTable(const std::string& str)
		{
			ClientClass* pClass = client->GetAllClasses();
			do
			{
				RecvTable* pRecvTable = pClass->m_pRecvTable;
				if(! pRecvTable )
					continue;
				if(!strcmp( pRecvTable->GetName(), str.c_str() ))
				{
					log.write(log.VERB, format("%s -> [0x%.8X]\n") %str.c_str()% (DWORD)pRecvTable );
					return pRecvTable;
				}
				pClass = pClass->m_pNext;
			}
			while ( pClass );
			log.write(log.WARN, format("Table %s not found\n") % str);
			return NULL;
		}

		RecvProp* findProp(RecvTable* pRecvTable, const std::string& str, int* x)
		{
			if(! pRecvTable) return NULL;
			for(int i = 0; i < pRecvTable->GetNumProps(); i++ )
			{
				RecvProp* pRecvProp = pRecvTable->GetProp(i);
				if(! pRecvProp )
					continue;
				if(!strcmp( pRecvProp->GetName(), str.c_str() ))
				{
					*x = pRecvProp->GetOffset();
					log.write(log.VERB, format("\t %s [0x%X]\n") % str % *x );
					return pRecvProp;
				}
			}
			log.write(log.WARN, boost::format("Property %s not found!\n") % str);
			return NULL;
		}

		DWORD findOffset(RecvTable* pRecvTable, const std::string& str)
		{
			int ret;
			findProp(pRecvTable, str, &ret);
			return (DWORD)ret;
		}

		DWORD get(const std::string& str) const
		{
			auto it = offsets.find(str);
			if (it != offsets.end())
			{
				log.write(log.VERB, format("%s [0x%.8X]\n") % str % it->second);
				return it->second;
			}
			log.write(log.WARN, format("NULL %s\n") % str);
			return 0;
		}

		DWORD operator[](const std::string& str) const
		{
			return get(str);
		}

		void dump(char* file=0)
		{
			offsets.insert(std::make_pair("", 0));

			ClientClass* pClass = client->GetAllClasses();
			log.write(log.INFO, format("ClientClass = 0x%X\n") % pClass);
			do
			{
				RecvTable* pRecvTable = pClass->m_pRecvTable;
				helper(pRecvTable);
				pClass = pClass->m_pNext;
			}
			while (pClass);
		}

		void helper(RecvTable* table, const std::string& parent = "")
		{
			const std::string tableName = table->GetName();
			for (int i = 0; i < table->GetNumProps(); i++)
			{
				RecvProp* prop = table->GetProp(i);
				if (!prop) continue;
				const std::string offsetName = prop->GetName();
				std::string concatStr = parent + "->" + tableName + "->" + offsetName;
				if (parent == "")
					concatStr = tableName + "->" + offsetName;
				auto newOffset = offsets.find(parent)->second + DWORD(prop->GetOffset());
				offsets.insert(std::make_pair(concatStr, (offsets.find(parent) == offsets.end() ? 0 : newOffset)));
				//if (concatStr.length() < 200) log.raw(boost::str(format("%s [0x%X]\n") % concatStr % newOffset));
					//log.write(log.VERB, format("%s\n") % concatStr); //LOG IT
				if (prop->GetDataTable())
				{
					std::string tmp;
					tmp = parent;
					if (tmp != "") tmp += "->";
					tmp += concatStr;
					if (prop->GetDataTable()->GetNumProps() > 0)
						helper(prop->GetDataTable(), tmp);
				}
			}
		}

#pragma endregion impl

		//CSGO stuff
		DWORD ply_Health, ply_LifeState, ply_Team, ply_Origin, ply_Flags, ply_ActiveWeapon;
		DWORD ply_EyePos;
		DWORD wep_Owner, wep_Clip1, wep_Clip2;
		DWORD c4_Ticking, c4_Blow, c4_TimerLen, c4_DefuseLen, c4_DefuseCountDown;

		IBaseClientDLL* client;

		static netvar* inst;

		std::map<std::string, DWORD> offsets;
	};
	netvar* netvar::inst;
}