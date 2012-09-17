#pragma once

#define CLIENT_DLL

//framework for source07

#include "../../required.h"
#include "../../ionbase.h"
#include "../../mem/vmt.h"
#include "../../render/source/sourcerender.h"

#include "sdk.h"

namespace ion
{
	class ionsource : public ionbase
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

		ionsource(const std::string& proj)
		{
			instance = this;
			auto root = superInit(proj);

			if (root.isNull()) return;

			modClient = module("client.dll");
			modEngine = module("engine.dll");

			fnClient = captureFactory("client.dll");
			fnEngine = captureFactory("engine.dll");
			fnVstdlib = captureFactory("vstdlib.dll");
			fnMatSurface = captureFactory("vguimatsurface.dll");
			fnVgui2 = captureFactory("vgui2.dll");

			appSystemFactory = **(CreateInterfaceFn**)sigs["AppSystemFactory"];
			spreadOffset = (DWORD)*(short*)sigs["WeaponSpread"];
			wepSeedOffset = (DWORD)*(short*)sigs["WeaponSeed"];

			gEngine = reinterpret_cast<IVEngineClient*>(appSystemFactory(getInterface("VEngineClient0"), NULL));
			gClient = reinterpret_cast<IBaseClientDLL*>(fnClient(getInterface("VClient0"), NULL));
			gCvar = reinterpret_cast<ICvar*>(appSystemFactory(getInterface("VEngineCvar0"), NULL));
			gPanel = reinterpret_cast<vgui::IPanel*>(appSystemFactory(getInterface("VGUI_Panel0"), NULL));
			gEnt = reinterpret_cast<IClientEntityList*>(fnClient(getInterface("VClientEntityList0"), NULL));
			gModelRender = reinterpret_cast<IVModelRender*>(appSystemFactory(getInterface("VEngineModel0"), NULL));
			gTrace = reinterpret_cast<IEngineTrace*>(appSystemFactory(getInterface("EngineTraceClient0"), NULL));
			gModelInfo = reinterpret_cast<IVModelInfoClient*>(appSystemFactory(getInterface("VModelInfoClient0"), NULL));
			gSurface = reinterpret_cast<vgui::ISurface*>(appSystemFactory(getInterface("VGUI_Surface0"), NULL));
			gMatSystem = reinterpret_cast<IMaterialSystem*>(appSystemFactory(getInterface("VMaterialSystem0"), NULL));
			DWORD* clientVmt = *(DWORD**)(gClient);
			gInput = (CInput*)**(DWORD**)(clientVmt[14] + 0x2); //InActivateMouse();

			render = new sourcerender(gSurface);
			lua.setDrawInstance(render);

			finishInit(root);

			if (gClient)
			{
				clientHk = new vmt(gClient);
				clientHk->hookMethod(&hkCreateMove, CLIENT_CREATEMOVE);
				clientHk->hookMethod(&hkDispatchUserMessage, CLIENT_DISPATCHUSERMESSAGE);
			}
			if (gPanel)
			{
				panelHk = new vmt(gPanel);
				panelHk->hookMethod(&hkPaintTraverse, PANEL_PAINTTRAVERSE);
			}
		}

		static bool __fastcall ionsource::hkDispatchUserMessage(void* thisptr, int edx, int msg_type, bf_read &msg_data )
		{
			if (msg_type == 5) return 1;
			instance->clientHk->unhookMethod(CLIENT_DISPATCHUSERMESSAGE);
			bool ret = instance->clientHk->getMethod<ionsource::DispatchUserMessageFn>(CLIENT_DISPATCHUSERMESSAGE)(thisptr, msg_type, msg_data);
			instance->clientHk->hookMethod(&hkDispatchUserMessage, CLIENT_DISPATCHUSERMESSAGE);
			lua.call("DUM");
			return ret;
		}

		static void __fastcall ionsource::hkCreateMove(void* thisptr, int edx, int sequence_number, float input_sample_frametime, bool active )
		{
			instance->clientHk->unhookMethod(CLIENT_CREATEMOVE);
			instance->clientHk->getMethod<ionsource::CreateMoveFn>(CLIENT_CREATEMOVE)(thisptr, sequence_number, input_sample_frametime, active);
			instance->clientHk->hookMethod(&hkCreateMove, CLIENT_CREATEMOVE);
			lua.call("CreateMove");
		}

		static void __fastcall ionsource::hkPaintTraverse(void* thisptr, int edx, VPANEL vguiPanel, bool forceRepaint, bool allowForce)
		{
			instance->panelHk->unhookMethod(PANEL_PAINTTRAVERSE);
			instance->panelHk->getMethod<ionsource::PaintTraverseFn>(PANEL_PAINTTRAVERSE)(thisptr, vguiPanel, forceRepaint, allowForce);
			instance->panelHk->hookMethod(&hkPaintTraverse, PANEL_PAINTTRAVERSE);
			auto panelName = instance->gPanel->GetName(vguiPanel);
			if (panelName && panelName[0] == 'F'&& panelName[5] == 'O' && panelName[12]=='P')
			{
				lua.call("Paint");
			}
		}

		static bool bTextCompare( const BYTE *pData, const char *pCompare )
		{
			for ( ; *pCompare; ++pData, ++pCompare )
				if ( *pData != *pCompare )
					return false;

			return true;
		}

		char *getInterface( char *pName)
		{
			for ( int i = 0; i < modClient.getLen(); i ++ )
				if ( bTextCompare( reinterpret_cast<PBYTE>(modClient.getStart() + i), pName ) )
				{
					log.write(log.VERB, format("%s\n")%std::string((char*)modClient.getStart() + i));
					return reinterpret_cast<char*>(modClient.getStart() + i);
				}
				return NULL;
		}

		CreateInterfaceFn captureFactory(char* mod)
		{
			CreateInterfaceFn fn = NULL;
			while( fn == NULL )
			{
				HMODULE hFactoryModule = GetModuleHandleA( mod );

				if( hFactoryModule )
				{
					fn = reinterpret_cast< CreateInterfaceFn >( GetProcAddress( hFactoryModule, "CreateInterface" ) );
				}
				Sleep( 10 );
			}
			return fn;
		}

		CreateInterfaceFn fnEngine, fnClient, fnVstdlib, fnMatSurface, fnVgui2, appSystemFactory;

		IBaseClientDLL* gClient;
		IVEngineClient* gEngine;
		ICvar *gCvar;
		vgui::ISurface* gSurface;
		vgui::IPanel* gPanel;
		IClientEntityList* gEnt;
		IVModelInfoClient* gModelInfo;
		CGlobalVarsBase* pGlobals;
		CInput* gInput;
		IEngineTrace* gTrace;
		IMaterialSystem* gMatSystem;
		IVModelRender* gModelRender;

		DWORD* pGlobalSpeed;
		DWORD wepSeedOffset;
		DWORD spreadOffset;

		module modClient, modEngine;

		vmt* clientHk, *inputHk, *panelHk;
		sourcerender* render;

		static ionsource* instance;
	};
	ionsource* ionsource::instance;
}