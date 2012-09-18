#pragma once

#define CLIENT_DLL

//framework for source07

#include "../../required.h"
#include "../../ionbase.h"
#include "../../mem/vmt.h"
#include "../../render/source/sourcerender.h"

#include "sdk.h"
#include "netvar.h"
#include "sourcehook.h"
#include "interfaces.h"
#include "entity.h"


namespace ion
{
	interfaces* source;
	class ionsource : public ionbase
	{
	public:
		ionsource(const std::string& proj)
		{
			source = new interfaces();
			instance = this;
			auto root = superInit(proj);

			if (root.isNull()) return;

			//bind
			lua.registerScope(
				luabind::class_<entity>("ent")
				.def("isValid", &entity::isValid)
				.def("isAlive", &entity::isAlive)
					.scope
					[
						luabind::def("me", &entity::me)
					]
			.def(luabind::const_self == luabind::other<entity>())
					);

			source->modClient = module("client.dll");
			source->modEngine = module("engine.dll");

			fnClient = captureFactory("client.dll");
			fnEngine = captureFactory("engine.dll");
			fnVstdlib = captureFactory("vstdlib.dll");
			fnMatSurface = captureFactory("vguimatsurface.dll");
			fnVgui2 = captureFactory("vgui2.dll");

			appSystemFactory = **(CreateInterfaceFn**)sigs["AppSystemFactory"];
			source->spreadOffset = (DWORD)*(short*)sigs["WeaponSpread"];
			source->wepSeedOffset = (DWORD)*(short*)sigs["WeaponSeed"];

			source->gEngine = reinterpret_cast<IVEngineClient*>(appSystemFactory(getInterface("VEngineClient0"), NULL));
			source->gClient = reinterpret_cast<IBaseClientDLL*>(fnClient(getInterface("VClient0"), NULL));
			source->gCvar = reinterpret_cast<ICvar*>(appSystemFactory(getInterface("VEngineCvar0"), NULL));
			source->gPanel = reinterpret_cast<vgui::IPanel*>(appSystemFactory(getInterface("VGUI_Panel0"), NULL));
			source->gEnt = reinterpret_cast<IClientEntityList*>(fnClient(getInterface("VClientEntityList0"), NULL));
			source->gModelRender = reinterpret_cast<IVModelRender*>(appSystemFactory(getInterface("VEngineModel0"), NULL));
			source->gTrace = reinterpret_cast<IEngineTrace*>(appSystemFactory(getInterface("EngineTraceClient0"), NULL));
			source->gModelInfo = reinterpret_cast<IVModelInfoClient*>(appSystemFactory(getInterface("VModelInfoClient0"), NULL));
			source->gSurface = reinterpret_cast<vgui::ISurface*>(appSystemFactory(getInterface("VGUI_Surface0"), NULL));
			source->gMatSystem = reinterpret_cast<IMaterialSystem*>(appSystemFactory(getInterface("VMaterialSystem0"), NULL));
			DWORD* clientVmt = *(DWORD**)(source->gClient);
			source->gInput = (CInput*)**(DWORD**)(clientVmt[14] + 0x2); //InActivateMouse();

			source->render = new sourcerender(source->gSurface);
			lua.setDrawInstance(source->render);

			source->nvar = new netvar(source->gClient);

			finishInit(root);

			if (source->gClient)
			{
				source->clientHk = new vmt(source->gClient);
				source->clientHk->hookMethod(&sourcehook::hkCreateMove, sourcehook::CLIENT_CREATEMOVE);
				source->clientHk->hookMethod(&sourcehook::hkDispatchUserMessage, sourcehook::CLIENT_DISPATCHUSERMESSAGE);
			}
			if (source->gPanel)
			{
				source->panelHk = new vmt(source->gPanel);
				source->panelHk->hookMethod(&sourcehook::hkPaintTraverse, sourcehook::PANEL_PAINTTRAVERSE);
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
			for ( int i = 0; i < source->modClient.getLen(); i ++ )
				if ( bTextCompare( reinterpret_cast<PBYTE>(source->modClient.getStart() + i), pName ) )
				{
					log.write(log.VERB, format("%s\n")%std::string((char*)source->modClient.getStart() + i));
					return reinterpret_cast<char*>(source->modClient.getStart() + i);
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

		static ionsource* instance;
	};
	ionsource* ionsource::instance;
}