#pragma once

#include "../../required.h"
#include "../../luamgr/luamgr.h"
#include "../../ionbase.h"
#include "../../mem/vmt.h"
#include "../../render/csgo/csgorender.h"

#include "sdk.h"
#include "netvar.h"
#include "interfaces.h"
#include "entity.h"
#include "vector.h"

static int once = false;
static ion::csgofont *tahoma;
namespace ion
{
	extern interfaces* csgo;
	class csgohook
	{
	public:
		const static int CLIENT_CREATEMOVE = 21;
		const static int CLIENT_DISPATCHUSERMESSAGE = 36;
		const static int PANEL_PAINTTRAVERSE = 41;

		typedef void (__thiscall *CreateMoveFn)(void*,int , float , bool  );
		typedef CUserCmd* (__thiscall *GetUserCmdFn)(void* thisptr,int sequence_number);
		typedef bool (__thiscall* DispatchUserMessageFn)( void* thisptr, int msg_type, bf_read& msg_data );
		typedef void (__thiscall *PaintTraverseFn)(void*,VPANEL, bool, bool);
		typedef void (__thiscall *AchievementMgrFn)(void*, float frametime);

		static bool __fastcall hkDispatchUserMessage(void* thisptr, int edx, int msg_type, bf_read &msg_data )
		{
			if (msg_type == 5) return 1;
			csgo->clientHk->unhookMethod(CLIENT_DISPATCHUSERMESSAGE);
			bool ret = csgo->clientHk->getMethod<csgohook::DispatchUserMessageFn>(CLIENT_DISPATCHUSERMESSAGE)(thisptr, msg_type, msg_data);
			csgo->clientHk->hookMethod(&hkDispatchUserMessage, CLIENT_DISPATCHUSERMESSAGE);
			lua.call("DUM");
			return ret;
		}

		static void __fastcall hkCreateMove(void* thisptr, int edx, int sequence_number, float input_sample_frametime, bool active )
		{
			csgo->clientHk->unhookMethod(CLIENT_CREATEMOVE);
			csgo->clientHk->getMethod<csgohook::CreateMoveFn>(CLIENT_CREATEMOVE)(thisptr, sequence_number, input_sample_frametime, active);
			csgo->clientHk->hookMethod(&hkCreateMove, CLIENT_CREATEMOVE);
			lua.call("CreateMove");
		}
		static void __fastcall hkPaintTraverse(void* thisptr, int edx, VPANEL vguiPanel, bool forceRepaint, bool allowForce)
		{
			csgo->panelHk->unhookMethod(PANEL_PAINTTRAVERSE);
			csgo->panelHk->getMethod<csgohook::PaintTraverseFn>(PANEL_PAINTTRAVERSE)(thisptr, vguiPanel, forceRepaint, allowForce);
			csgo->panelHk->hookMethod(&hkPaintTraverse, PANEL_PAINTTRAVERSE);
			auto panelName = csgo->gPanel->GetName(vguiPanel);

			if (panelName && panelName[0] == 'F'&& panelName[5] == 'O' && panelName[12]=='P')
			{
				if (csgo->gEngine->IsInGame()) glua.inGame = true;
				else glua.inGame = false;
				//lua.call("Paint");
				auto r = csgo->render;
				if (!once)
				{
					once = true;
					tahoma = (csgofont*)csgo->render->createFont("Tahoma", 12, csgofont::FONTFLAG_OUTLINE, 650);
				}
				size sz;
				int x = 10, y = 10;
				if (glua.inGame)
				{
					sz = r->renderText(0, tahoma, point(x, y), color::red(), format("In Game: %d") % csgo->gEnt->GetHighestEntityIndex());
					y += sz.getHeight();
					for (int i = 1; i < csgo->gEnt->GetHighestEntityIndex(); i++)
					{
						IClientEntity* clientEntity = csgo->gEnt->GetClientEntity(i);
						auto baseEnt = clientEntity ?  clientEntity->GetBaseEntity() : 0;
						IClientNetworkable* network = csgo->gEnt->GetClientNetworkable(i);
						auto cc = !network ? 0 : network->GetClientClass();
						typedef void*( __cdecl* OrigFn)(void*);
						auto name = cc ? cc->GetName() : "";
						Vector v = Vector(); //!network?Vector() :*makeptr<Vector>(network, csgo->nvar->ply_Origin);
						sz = r->renderText(0, tahoma, point(x, y), color::red(), format("Ent 0x%X n0x%X n20x%X %d 0x%X 0x%X %d %s %f %f %f") % csgo->gEnt %network%0%i % clientEntity % baseEnt % (baseEnt?baseEnt->index:-69) % name % v.x %v.y %v.z);
						y += sz.getHeight();
					}
				}
				else
				{
					r->renderText(0, tahoma, point(10, 10), color::red(), "Not in game");
				}
			}
		}
	};
}