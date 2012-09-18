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
	extern interfaces* source;

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

		static CBaseEntity* getBaseEnt(int i)
		{
			auto pEnt = source->gEnt->GetClientEntity(i);
			if (!pEnt)
			{
				//log.write(log.WARN, format("Entity %d NULL\n") % i, "GetBaseEntity");
				return NULL;
			}
			return pEnt->GetBaseEntity();
		}

		bool isPlayer() const
		{
			if (!ent) return false;
			//CTFPlayer
			const char* name = ent->GetClientClass()->GetName();
			if (!name) return false;
			return name[2] == 'F' && name[3] == 'P' && name[8] == 'r' && name[9] == '\0';
		}

		const vector getOrigin() const
		{
			if (!isValid()) return vector();
			auto old = ent->GetAbsOrigin();
			return vector(old.x, old.y, old.z);
		}

		int getTeam() const
		{
			return *makeptr<int>(ent, source->nvar->ent_Team);
		}

		static entity getBaseEntAsEntity(int i)
		{
			auto pEnt = source->gEnt->GetClientEntity(i);
			if (!pEnt)
			{
				//log.write(log.WARN, format("Entity %d NULL\n") % i, "GetBaseEntity");
				return entity(0);
			}
			return entity(pEnt->GetBaseEntity());
		}

		static int getHighestEntityIndex()
		{
			return source->gEnt->GetHighestEntityIndex();
		}

		bool operator==(const entity& other) const
		{
			return other.ent == ent;
		}

		bool isBot() const
		{
			if (!isValid()) return false;
			player_info_s pinfo;
			if (!source->gEngine->GetPlayerInfo(ent->index, &pinfo))
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
			if (!source->gEngine->GetPlayerInfo(ent->index, &pinfo))
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
			return *makeptr<int>(ent, source->nvar->ply_Health);
		}

		static entity me()
		{
			return entity(getBaseEnt(source->gEngine->GetLocalPlayer()));
		}

		bool isAlive() const
		{
			return isValid() && !ent->IsDormant() && *makeptr<char>(ent, source->nvar->ply_LifeState) == LIFE_ALIVE;
		}

		C_BaseEntity* ent;
	};
};