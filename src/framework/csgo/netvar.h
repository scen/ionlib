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

			t = findTable("DT_BaseEntity");
			ent_Team = findOffset(t, "m_iTeamNum");

			t = findTable("DT_BaseCombatCharacter");
			ply_ActiveWeapon = findOffset(t, "m_hActiveWeapon");
			ply_Weapons = findOffset(t, "m_hMyWeapons");
		}
		~netvar();

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
				offsets.insert(std::make_pair(concatStr, (offsets.find(parent) == offsets.end() ? 0 : offsets.find(parent)->second + DWORD(prop->GetOffset()))));
				if (concatStr.length() < 200) log.raw(boost::str(format("%s\n") % concatStr));
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



		//TF2 stuff
		DWORD ply_Class, ply_LifeState, ply_Health, ply_TfCond,
			ply_ActiveWeapon, ply_Weapons, ply_GlowEnabled, ply_Shared, ply_medigunCharge, ply_EyePos,
			ply_EyeAngles1;
		DWORD spy_DTeam, spy_DClass, spy_Backstab;
		DWORD obj_Level, obj_Builder, obj_Health, obj_Sapper, obj_Mini, obj_MaxHealth, obj_Metal,
			obj_Building, obj_Mode, obj_Disabled, obj_Percent;
		DWORD sentry_Wrangle, sentry_Shield, sentry_Rockets, sentry_Shells;
		DWORD ent_Team;
		DWORD off_AttributeManager, off_item, off_ItemDefIdx;
		DWORD wep_lastFireTime, wep_lastCritCheckTime, wep_ammoType;
		DWORD bomb_Type, bomb_Touched, bomb_Owner, bomb_Critical;


		IBaseClientDLL* client;

		static netvar* inst;

		std::map<std::string, DWORD> offsets;
	};
	netvar* netvar::inst;
}