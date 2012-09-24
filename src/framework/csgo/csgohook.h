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
#include "visuals.h"

namespace ion
{
	extern interfaces* csgo;
	class csgohook
	{
	public:
		const static int CLIENT_CREATEMOVE = 21;
		const static int CLIENT_DISPATCHUSERMESSAGE = 36;
		const static int PANEL_PAINTTRAVERSE = 41;
		const static int MODELRENDER_DRAWMODELEXECUTE = 19;

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

		static void __stdcall hkDrawModelExecute( IMatRenderContext * ctx, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld)
		{
			csgo->modelRenderHk->unhookMethod(MODELRENDER_DRAWMODELEXECUTE);
			if (pInfo.pModel)
			{
				auto name = csgo->gModelInfo->GetModelName(pInfo.pModel);
				//log.write(log.VERB, format("%s\n") %name);
				//012345678901234567
				//models/weapons/t_arms
				//models/weapons/ct_arms
				if (!strncmp(name + 17, "arms", 4) || !strncmp(name + 18, "arms", 4))
				{
					//nodraw
				}
				else
				{
					csgo->gModelRender->DrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld);
				}
			}
			else
			{
				csgo->gModelRender->DrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld);
			}

			csgo->modelRenderHk->hookMethod(&hkDrawModelExecute, MODELRENDER_DRAWMODELEXECUTE);
		}

		static void __fastcall hkCreateMove(void* thisptr, int edx, int sequence_number, float input_sample_frametime, bool active )
		{
			csgo->clientHk->unhookMethod(CLIENT_CREATEMOVE);
			csgo->clientHk->getMethod<csgohook::CreateMoveFn>(CLIENT_CREATEMOVE)(thisptr, sequence_number, input_sample_frametime, active);
			csgo->clientHk->hookMethod(&hkCreateMove, CLIENT_CREATEMOVE);

			if (!entity::me().isAlive()) return;
			log.write(log.WARN, format("gInput = 0x%X gClient = 0x%X 0x%X\n") % csgo->gInput% csgo->gClient% getvfunc<const void*>(csgo->gInput, 8) );
			return;
			CUserCmd* pCmd = csgo->gInput->GetUserCmd(0, sequence_number);

			if (!pCmd) return;

			for (int i = 1; i < 50; i++) //only have to loop up to max players
			{

			}

			CVerifiedUserCmd* pVerifiedCmd = *(CVerifiedUserCmd**)((DWORD)csgo->gInput + 0xB8);
			CVerifiedUserCmd* pVerified = &pVerifiedCmd[ sequence_number % MULTIPLAYER_BACKUP];
			memcpy(&pVerified->m_cmd, pCmd, sizeof(CUserCmd));

			pVerified->m_crc = CRC32_ProcessSingleBuffer(&pVerified->m_cmd, sizeof(CUserCmd));
		}
		static void __fastcall hkPaintTraverse(void* thisptr, int edx, VPANEL vguiPanel, bool forceRepaint, bool allowForce)
		{
			csgo->panelHk->unhookMethod(PANEL_PAINTTRAVERSE);
			csgo->panelHk->getMethod<csgohook::PaintTraverseFn>(PANEL_PAINTTRAVERSE)(thisptr, vguiPanel, forceRepaint, allowForce);
			csgo->panelHk->hookMethod(&hkPaintTraverse, PANEL_PAINTTRAVERSE);
			auto panelName = csgo->gPanel->GetName(vguiPanel);

			if (panelName && panelName[0] == 'F'&& panelName[5] == 'O' && panelName[12] == 'P')
			{
				//if (csgo->gEngine->IsInGame()) glua.inGame = true;
				//else glua.inGame = false;
				//lua.call("Paint");
				auto me = entity::me();
				if (csgo->gEngine->IsInGame() && me.isAlive())
				{
					for (int i = 1; i < csgo->gEnt->GetHighestEntityIndex(); i++)
					{
						entity ent(i);
						if (ent == me) continue;
						
						switch (ent.getType())
						{
						case entity::PLAYER:
							visuals::renderPlayer(ent);
							break;
						case entity::C4PLANTED:
							visuals::renderPlantedC4(ent);
							break;
						case entity::C4:
							visuals::renderC4(ent);
							break;
						case entity::WEAPON:
							visuals::renderWeapon(ent);
							break;
						default:
							break;
							if (!ent.isDormant())
							{
								auto pos = ent.getBonePos(0).toScreen();
								if (pos.visible)
								{
									csgo->render->renderText(render::Center, csgo->tahoma12, point(pos.x, pos.y), color::white(), ent.getClientClassName());
								}
							}
							break;
						}
					}
				
				}
				else
				{
				}
			}
		}
	};
}