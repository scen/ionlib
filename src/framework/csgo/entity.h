#pragma once

#include "../../required.h"
#include "../../util/log.h"
#include "../../mem/common.h"
#include "sdk.h"
#include "netvar.h"
#include "interfaces.h"
#include "vector.h"

//Source engine entity

namespace ion
{
	extern interfaces* csgo;

	class entity
	{
	public:
		enum EEntityType
		{
			PLAYER = 0,

		};
		entity(C_BaseEntity* e) : ent(e)
		{
		}
		~entity()
		{
			// log.write(log.WARN, format("Entity destructor called 0x%X\n") % ent);
		}
		bool isValid() const
		{
			return ent != NULL;
		}

		static C_BaseEntity* getBaseEnt(int i)
		{
			auto pEnt = csgo->gEnt->GetClientEntity(i);
			if (!pEnt)
			{
				//log.write(log.WARN, format("Entity %d NULL\n") % i, "GetBaseEntity");
				return NULL;
			}
			return pEnt->GetBaseEntity();
		}

		int getType() const
		{
			if (!ent) return false;
			return PLAYER;
		}

		const char* getClassName() const
		{
			if (!ent) return "";
			log.write(log.WARN, format("%d %d\n") % ent->index % *makeptr<DWORD>(ent, 88));
			auto p = csgo->gEnt->GetClientEntity(ent->index);
			if (!p) return "";
			auto p2 = p->GetClientClass();
			if (!p2) return "";
			return p2->GetName();
		}

		const vector getOrigin() const
		{
			if (!isValid()) return vector();
			return *makeptr<vector>(ent, csgo->nvar->ply_Origin);
		}

		int getTeam() const
		{
			return *makeptr<int>(ent, csgo->nvar->ply_Team);
		}

		static entity getBaseEntAsEntity(int i)
		{
			auto pEnt = csgo->gEnt->GetClientEntity(i);
			if (!pEnt)
			{
				//log.write(log.WARN, format("Entity %d NULL\n") % i, "GetBaseEntity");
				return entity(0);
			}
			return entity(pEnt->GetBaseEntity());
		}

		static int getHighestEntityIndex()
		{
			return csgo->gEnt->GetHighestEntityIndex();
		}

		bool operator==(const entity& other) const
		{
			return other.ent == ent;
		}

		bool isBot() const
		{
			if (!isValid()) return false;
			player_info_s pinfo;
			if (!csgo->gEngine->GetPlayerInfo(ent->index, &pinfo))
			{
				log.write(log.WARN, "GetPlayerInfo failed");
				return false;
			}
			return pinfo.fakeplayer;
		}

		std::string getName() const
		{
			if (!isValid())
			{
				log.write(log.WARN, "Entity is NULL\n", "getName");
			}
			player_info_s pinfo;
			if (!csgo->gEngine->GetPlayerInfo(ent->index, &pinfo))
			{
				log.write(log.WARN, "GetPlayerInfo failed");
				return "";
			}
			if (pinfo.fakeplayer)
			{
				return pinfo.name;
			}
			return pinfo.friendsName;
		}

		int getHealth() const
		{
			if (!isAlive()) return 0;
			return *makeptr<int>(ent, csgo->nvar->ply_Health);
		}

		static entity me()
		{
			return entity(getBaseEnt(csgo->gEngine->GetLocalPlayer()));
		}

		bool isDormant() const
		{
			if (*makeptr<int>(ent, 0x58) != -1)
				return *makeptr<BYTE>(ent, 0xDD);
			return false;
		}

		bool isAlive() const
		{
			return isValid() && !isDormant() && *makeptr<char>(ent, csgo->nvar->ply_LifeState) == LIFE_ALIVE;
		}

		C_BaseEntity* ent;
	};
};