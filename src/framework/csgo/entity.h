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

	class weapon;

	class entity
	{
	public:
		static const unsigned int TRACE_FLAGS_ISVISIBLE = 0x4600400B;
		static const entity invalid;

		enum EEntityType
		{
			UNKNOWN = 0,
			PLAYER,
			C4,
			C4PLANTED
		};

		enum ETeamID
		{
			TERRORIST = 2,
			COUNTERTERRORIST = 3
		};

		enum EBonePos
		{
			BONEMIN = 0,
			BONEMAX,
			BONEAVG 
		};

		enum EPlayerBones
		{
			PELVIS = 0,
			LEFT_THIGH,
			LEFT_ANKLE,
			LEFT_FOOT,
			RIGHT_THIGH,
			RIGHT_ANKLE,
			RIGHT_FOOT,
			STOMACH,
			MID_CHEST,
			UPPER_CHEST,
			NECK,
			HEAD,
			LEFT_SHOULDER,
			LEFT_ELBOW,
			LEFT_HAND,
			RIGHT_SHOULDER,
			RIGHT_ELBOW,
			RIGHT_HAND
		};
		entity(int iidx) : idx(iidx), network(0), clientEntity(0), baseEnt(0)
		{
			network = csgo->gEnt->GetClientNetworkable(idx);
			clientEntity = csgo->gEnt->GetClientEntity(idx);
			if (clientEntity) baseEnt = clientEntity->GetBaseEntity();
		}

		entity(C_BaseEntity* ent) : baseEnt(ent)
		{
			idx = getIndex(ent);
			network = csgo->gEnt->GetClientNetworkable(idx);
			clientEntity = csgo->gEnt->GetClientEntity(idx);

		}

		entity() : baseEnt(0), clientEntity(0), idx(-1), network(0)
		{

		}
		~entity()
		{
			// log.write(log.WARN, format("Entity destructor called 0x%X\n") % ent);
		}

		bool operator!=(const entity& other) const
		{
			return !(*this == other);
		}

		//static for quicker access if you ONLY want the index
		static int getIndex(C_BaseEntity* ent)
		{
			if (!ent) return -1;
			return *makeptr<int>(ent, 0x64);
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

			if (!strcmp(name, "CC4")) return C4;

			//CPlantedC4
			//0123456789
			if (name[1] == 'P' && name[4] == 'n' && name[9] == '4') return C4PLANTED;

			return UNKNOWN;
		}

		float distanceTo() const
		{
			return fabs((me().getOrigin() - getOrigin()).Length());
		}

		const vector getOrigin() const
		{
			if (!isValid()) return vector::empty;
			return *makeptr<vector>(baseEnt, csgo->nvar->ply_Origin);
		}

		int getTeam() const
		{
			if (!isValid()) return -1;
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
			if (!csgo->gEngine->GetPlayerInfo(idx, &pinfo))
			{
				return false;
			}
			return pinfo.fakeplayer;
		}

		std::string getName() const
		{
			if (!isValid()) return "";
			player_info_s pinfo;
			if (!csgo->gEngine->GetPlayerInfo(idx, &pinfo))
			{
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
			if (!isValid()) return true;
			if (*makeptr<int>(baseEnt, 0x58) != -1)
				return (*makeptr<BYTE>(baseEnt, 0xDD) & 1) == 1;
			return false;
		}

		int getFlags() const
		{
			if (!isValid()) return 0;
			return *makeptr<int>(baseEnt, csgo->nvar->ply_Flags);
		}

		vector getBonePos(int bone, int type = BONEAVG) const
		{
			if (isDormant()) return vector::empty;

			matrix3x4a_t pmatrix[MAXSTUDIOBONES];

			auto model = clientEntity->GetModel();
			if (!model) return vector::empty;

			auto pStudioHdr = csgo->gModelInfo->GetStudiomodel(model);
			if (!pStudioHdr) return vector::empty;

			if (!clientEntity->SetupBones(pmatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, csgo->gGlobalVars->curtime)) return vector::empty;

			auto set = pStudioHdr->pHitboxSet(0);
			if (!set) return vector::empty;

			mstudiobbox_t* pBox = pStudioHdr->pHitbox(bone, 0);
			if (!pBox) return vector::empty;

			if (type == BONEMIN)
			{
				vector ret;
				VectorTransform(pBox->bbmin, pmatrix[pBox->bone], ret);
				return ret;
			}
			if (type == BONEMAX)
			{
				vector ret;
				VectorTransform(pBox->bbmax, pmatrix[pBox->bone], ret);
				return ret;
			}
			Vector min, max;
			vector vRet;
			VectorTransform(pBox->bbmin, pmatrix[pBox->bone], min);
			VectorTransform(pBox->bbmax, pmatrix[pBox->bone], max);
			vRet = (vector(min) + vector(max)) * 0.5f;
			return vRet;
		}

		//only for local player
		vector getEyePos() const
		{
			if (!isAlive()) return vector::empty;
			return getOrigin() + *makeptr<vector>(baseEnt, csgo->nvar->ply_EyePos);
		}

		C_BaseEntity* getActiveWeapon() const
		{
			if (!isAlive()) return 0;
			EHANDLE ehandle = *makeptr<EHANDLE>(baseEnt, csgo->nvar->ply_ActiveWeapon);
			auto c = csgo->gEnt->GetClientEntityFromHandle(ehandle);
			if (!c) return 0;
			return (C_BaseEntity*)c;
		}

		class CTraceFilter : public ITraceFilter
		{
		public:
			CTraceFilter(void* skip) : ITraceFilter(), pSkip(skip)
			{
			}
			bool ShouldHitEntity( IHandleEntity* pEntityHandle, int contentsMask )
			{
				return !( pEntityHandle == pSkip );
			}

			virtual TraceType_t GetTraceType() const
			{
				return TRACE_EVERYTHING;
			}

			void* pSkip;
		};

		bool isVisible(int bone) const
		{
			if (isDormant() || me().isDormant()) return false;
			Ray_t ray;
			trace_t tr;
			CTraceFilter traceFilter(me().baseEnt);
			ray.Init(me().getEyePos(), getBonePos(bone));
			csgo->gTrace->TraceRay(ray, TRACE_FLAGS_ISVISIBLE, &traceFilter, &tr);
			if (tr.fraction == 1.0f) return true;
			if (tr.m_pEnt  && tr.m_pEnt == baseEnt) return true;
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
	const entity entity::invalid = entity();
};