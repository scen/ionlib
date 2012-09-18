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

			t = findTable("DT_TFPlayer");
			ply_GlowEnabled = findOffset(t, "m_bGlowEnabled");
			ply_Shared = findOffset(t, "m_Shared");

			t = findTable("DT_TFProjectile_Pipebomb");
			bomb_Type = findOffset(t, "m_iType");
			bomb_Touched = findOffset(t, "m_bTouched");
			bomb_Owner = findOffset(t, "m_hLauncher");

			t = findTable("DT_BaseObject");
			obj_Health = findOffset(t, "m_iHealth");
			obj_Level = findOffset(t, "m_iUpgradeLevel");
			obj_Builder = findOffset(t, "m_hBuilder");
			obj_Sapper = findOffset(t, "m_bHasSapper");
			obj_Building = findOffset(t, "m_bBuilding");
			obj_Mini = findOffset(t, "m_bMiniBuilding");
			obj_Metal = findOffset(t, "m_iUpgradeMetal");
			obj_Mode = findOffset(t, "m_iObjectMode");
			obj_MaxHealth = findOffset(t, "m_iMaxHealth");
			obj_Disabled = findOffset(t, "m_bDisabled");
			obj_Percent = findOffset(t, "m_flPercentageConstructed");

			t = findTable("DT_ObjectSentrygun");
			sentry_Wrangle = findOffset(t, "m_bPlayerControlled");
			sentry_Shield = findOffset(t, "m_bShielded");
			sentry_Shells = findOffset(t, "m_iAmmoShells");
			sentry_Rockets = findOffset(t, "m_iAmmoRockets");

			t = findTable("DT_EconEntity");
			p = findProp( t, "m_AttributeManager", (int*)&off_AttributeManager);
			p = findProp( p->GetDataTable(), "m_Item", (int*)&off_item);
			off_ItemDefIdx = findOffset(p->GetDataTable(), "m_iItemDefinitionIndex");


			//Fix later TODO
			wep_lastFireTime = get("DT_TFWeaponBase->LocalActiveTFWeaponData->DT_LocalTFWeaponData->m_flLastFireTime");
			wep_lastCritCheckTime = get("DT_TFWeaponBase->LocalActiveTFWeaponData->DT_LocalTFWeaponData->m_flLastCritCheckTime");
			wep_ammoType = get("DT_BaseCombatWeapon->LocalWeaponData->DT_LocalWeaponData->m_iPrimaryAmmoType");

			spy_DTeam = get("DT_TFPlayer->m_Shared->DT_TFPlayerShared->m_nDisguiseTeam");
			spy_DClass = get("DT_TFPlayer->m_Shared->DT_TFPlayerShared->m_nDisguiseClass");

			ply_TfCond = get("DT_TFPlayer->m_Shared->DT_TFPlayerShared->m_nPlayerCond");
			ply_Class = get("DT_TFPlayer->m_PlayerClass->DT_TFPlayerClassShared->m_iClass");
			ply_medigunCharge = get("DT_WeaponMedigun->LocalTFWeaponMedigunData->DT_LocalTFWeaponMedigunData->m_flChargeLevel");
			ply_EyePos = get("DT_BasePlayer->localdata->DT_LocalPlayerExclusive->m_vecViewOffset[0]");
			ply_EyeAngles1 = get("DT_TFPlayer->tfnonlocaldata->DT_TFNonLocalPlayerExclusive->m_angEyeAngles[1]");
			bomb_Critical = get("DT_TFProjectile_Pipebomb->baseclass->DT_TFWeaponBaseGrenadeProj->m_bCritical");
			spy_Backstab = get("DT_TFWeaponKnife->m_bReadyToBackstab");
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
				//if (concatStr.length() < 110)
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