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
			UNKNOWN = 0,
			PLAYER,
		};
		entity(int iidx) : idx(iidx), network(0), clientEntity(0), baseEnt(0)
		{
			network = csgo->gEnt->GetClientNetworkable(idx);
			clientEntity = csgo->gEnt->GetClientEntity(idx);
			if (clientEntity) baseEnt = clientEntity->GetBaseEntity();
		}
		~entity()
		{
			// log.write(log.WARN, format("Entity destructor called 0x%X\n") % ent);
		}

		const char* getClientClassName() const
		{
			if (!isValid()) return "";
			auto cc = network->GetClientClass();
			if (!cc) return "";
			return cc->GetName();
		}

		bool isValid() const
		{
			return network && clientEntity;
		}

		int getType() const
		{
			if (!isValid()) return 0;
			//CCSPlayer
			//012345678
			auto name = getClientClassName();
			if (!name) return UNKNOWN;

			if (name[1] == 'C' && name[3] == 'P' && name[8] == 'r') return PLAYER;

			return UNKNOWN;
		}


		const vector getOrigin() const
		{
			if (!isValid()) return vector::empty;
			return *makeptr<vector>(baseEnt, csgo->nvar->ply_Origin);
		}

		int getTeam() const
		{
			return *makeptr<int>(baseEnt, csgo->nvar->ply_Team);
		}

		static int getHighestEntityIndex()
		{
			return csgo->gEnt->GetHighestEntityIndex();
		}

		bool operator==(const entity& other) const
		{
			return other.baseEnt == baseEnt;
		}

		bool isBot() const
		{
			if (!isValid()) return false;
			player_info_s pinfo;
			if (!csgo->gEngine->GetPlayerInfo(baseEnt->index, &pinfo))
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
			if (!csgo->gEngine->GetPlayerInfo(baseEnt->index, &pinfo))
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
			return *makeptr<int>(baseEnt, csgo->nvar->ply_Health);
		}

		static entity me()
		{
			return entity(csgo->gEngine->GetLocalPlayer());
		}

		bool isDormant() const
		{
			if (*makeptr<int>(baseEnt, 0x58) != -1)
				return *makeptr<BYTE>(baseEnt, 0xDD);
			return false;
		}

		bool isAlive() const
		{
			return isValid() && !isDormant() && *makeptr<char>(baseEnt, csgo->nvar->ply_LifeState) == LIFE_ALIVE;
		}

		int idx;
		C_BaseEntity* baseEnt;
		IClientNetworkable* network;
		IClientEntity* clientEntity;
	};
};