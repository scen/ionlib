#pragma once

#include "../../required.h"
#include "../../util/log.h"
#include "../../mem/common.h"
#include "sdk.h"
#include "netvar.h"
#include "interfaces.h"

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
		entity(CBaseEntity* e) : ent(e)
		{
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
				log.write(log.WARN, format("Entity %d NULL\n") % i, "GetBaseEntity");
				return NULL;
			}
			return pEnt->GetBaseEntity();
		}

		bool operator==(const entity& other) const
		{
			return other.ent == ent;
		}

		const char* getName() const
		{
			player_info_s pinfo;
			if (!source->gEngine->GetPlayerInfo(ent->index, &pinfo))
			{
				log.write(log.WARN, "GetPlayerInfo failed");
				return "";
			}
			return pinfo.friendsName;
		}

		static entity* me()
		{
			log.write(log.VERB, "me()\n");
			return (entity*)getBaseEnt(source->gEngine->GetLocalPlayer());
		}

		bool isAlive() const
		{
			return isValid() && !ent->IsDormant() && *makeptr<char>(ent, source->nvar->ply_LifeState) == LIFE_ALIVE;
		}

		C_BaseEntity* ent;
	};
};