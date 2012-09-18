#pragma once

#include "../../required.h"
#include "../../luamgr/luamgr.h"
#include "../../ionbase.h"
#include "../../mem/vmt.h"
#include "../../render/source/sourcerender.h"

#include "sdk.h"
#include "netvar.h"
#include "interfaces.h"

namespace ion
{
	extern interfaces* source;
	class sourcehook
	{
	public:
		const static int CLIENT_CREATEMOVE = 21;
		const static int CLIENT_DISPATCHUSERMESSAGE = 36;
		const static int PANEL_PAINTTRAVERSE = 40;

		typedef void (__thiscall *CreateMoveFn)(void*,int , float , bool  );
		typedef CUserCmd* (__thiscall *GetUserCmdFn)(void* thisptr,int sequence_number);
		typedef bool (__thiscall* DispatchUserMessageFn)( void* thisptr, int msg_type, bf_read& msg_data );
		typedef void (__thiscall *PaintTraverseFn)(void*,VPANEL, bool, bool);
		typedef void (__thiscall *AchievementMgrFn)(void*, float frametime);

		static bool __fastcall hkDispatchUserMessage(void* thisptr, int edx, int msg_type, bf_read &msg_data )
		{
			if (msg_type == 5) return 1;
			source->clientHk->unhookMethod(CLIENT_DISPATCHUSERMESSAGE);
			bool ret = source->clientHk->getMethod<sourcehook::DispatchUserMessageFn>(CLIENT_DISPATCHUSERMESSAGE)(thisptr, msg_type, msg_data);
			source->clientHk->hookMethod(&hkDispatchUserMessage, CLIENT_DISPATCHUSERMESSAGE);
			lua.call("DUM");
			return ret;
		}

		static void __fastcall hkCreateMove(void* thisptr, int edx, int sequence_number, float input_sample_frametime, bool active )
		{
			source->clientHk->unhookMethod(CLIENT_CREATEMOVE);
			source->clientHk->getMethod<sourcehook::CreateMoveFn>(CLIENT_CREATEMOVE)(thisptr, sequence_number, input_sample_frametime, active);
			source->clientHk->hookMethod(&hkCreateMove, CLIENT_CREATEMOVE);
			lua.call("CreateMove");
		}

		static void __fastcall hkPaintTraverse(void* thisptr, int edx, VPANEL vguiPanel, bool forceRepaint, bool allowForce)
		{
			source->panelHk->unhookMethod(PANEL_PAINTTRAVERSE);
			source->panelHk->getMethod<sourcehook::PaintTraverseFn>(PANEL_PAINTTRAVERSE)(thisptr, vguiPanel, forceRepaint, allowForce);
			source->panelHk->hookMethod(&hkPaintTraverse, PANEL_PAINTTRAVERSE);
			auto panelName = source->gPanel->GetName(vguiPanel);
			if (panelName && panelName[0] == 'F'&& panelName[5] == 'O' && panelName[12]=='P')
			{
				lua.call("Paint");
			}
		}
	};
}