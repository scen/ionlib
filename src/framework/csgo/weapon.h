#pragma once

#include "../../required.h"
#include "../../util/log.h"
#include "../../mem/common.h"
#include "sdk.h"
#include "netvar.h"
#include "interfaces.h"
#include "vector.h"
#include "entity.h"

//weapon

namespace ion
{
	extern interfaces* csgo;

	class weapon: public entity
	{
	public:
		enum EWeaponID
		{
			WEAPON_DEAGLE = 1,
			WEAPON_ELITE,
			WEAPON_FIVESEVEN,
			WEAPON_GLOCK,
			WEAPON_AK47 = 7,
			WEAPON_AUG,
			WEAPON_AWP,
			WEAPON_FAMAS,
			WEAPON_G3SG1,
			WEAPON_GALILAR = 13,
			WEAPON_M249,
			WEAPON_M4A1 = 16,
			WEAPON_MAC10,
			WEAPON_P90 = 19,
			WEAPON_UMP45 = 24,
			WEAPON_XM1014,
			WEAPON_BIZON,
			WEAPON_MAG7,
			WEAPON_NEGEV,
			WEAPON_SAWEDOFF,
			WEAPON_TEC9,
			WEAPON_TASER,
			WEAPON_HKP2000,
			WEAPON_MP7,
			WEAPON_MP9,
			WEAPON_NOVA,
			WEAPON_P250,
			WEAPON_SCAR20 = 38,
			WEAPON_SG556,
			WEAPON_SSG08,
			WEAPON_KNIFE_ARMSRACE,
			WEAPON_KNIFE,
			WEAPON_FLASHBANG,
			WEAPON_HEGRENADE,
			WEAPON_SMOKEGRENADE,
			WEAPON_MOLOTOV,
			WEAPON_DECOY,
			WEAPON_INCGRENADE,
			WEAPON_C4 = 49,
		};  

		weapon() : entity() {}
		weapon(int i) : entity(i) {}
		weapon(C_BaseEntity* ent) : entity(ent) {}
		weapon(C_BaseCombatWeapon* wep) : entity((C_BaseEntity*)wep) {}

		int getID() const
		{
			if (!isValid()) return -1;
			typedef int(__thiscall* GetCSWeaponIDFn)(void* pthis);
			return getvfunc<GetCSWeaponIDFn>(baseEnt, 380)(baseEnt);
		}

		const char* getWeaponAlias() const
		{
			if (!isValid()) return "";
			typedef const char*(__cdecl* WeaponIdAsStringFn)(int id);
			static WeaponIdAsStringFn weaponIDAsString = (WeaponIdAsStringFn)csgo->weaponIDAsString;
			return weaponIDAsString(getID());
		}

		entity getPlayerOwner()
		{
			if (!isValid()) return 0;
			typedef C_BaseEntity*(__thiscall* GetPlayerOwnerFn)(void *pthis);
			return getvfunc<GetPlayerOwnerFn>(baseEnt, 396)(baseEnt);
		}

		const char* getNiceName() const
		{
			return getWeaponAlias() + 7; // TODO
		}
	};
};