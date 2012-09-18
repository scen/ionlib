#pragma once

#define CLIENT_DLL

//framework for csgo/swarm engine

#include "../../required.h"
#include "../../ionbase.h"
#include "../../mem/vmt.h"
//#include "../../render/source/sourcerender.h"

#include "sdk.h"
#include "csgohook.h"
#include "interfaces.h"


namespace ion
{
	interfaces* source;
	class ioncsgo : public ionbase
	{
	public:
		ioncsgo(const std::string& proj)
		{
			source = new interfaces();
			instance = this;
			auto root = superInit(proj);

			if (root.isNull()) return;

			source->modClient = module("client.dll");
			source->modEngine = module("engine.dll");

			fnClient = captureFactory("client.dll");
			fnEngine = captureFactory("engine.dll");
			fnVstdlib = captureFactory("vstdlib.dll");
			fnMatSurface = captureFactory("vguimatsurface.dll");
			fnVgui2 = captureFactory("vgui2.dll");

			//appSystemFactory = **(CreateInterfaceFn**)sigs["AppSystemFactory"];
			//source->spreadOffset = (DWORD)*(short*)sigs["WeaponSpread"];
			//source->wepSeedOffset = (DWORD)*(short*)sigs["WeaponSeed"];

			/*source->gEngine = */reinterpret_cast<IVEngineClient*>(appSystemFactory(getInterface("VEngineClient0"), NULL));
			/*source->gClient = */reinterpret_cast<IBaseClientDLL*>(fnClient(getInterface("VClient0"), NULL));
			/*source->gCvar = */reinterpret_cast<ICvar*>(appSystemFactory(getInterface("VEngineCvar0"), NULL));
			/*source->gPanel = */reinterpret_cast<vgui::IPanel*>(appSystemFactory(getInterface("VGUI_Panel0"), NULL));
			/*source->gEnt = */reinterpret_cast<void*>(fnClient(getInterface("VClientEntityList0"), NULL));
			/*source->gModelRender = */reinterpret_cast<IVModelRender*>(appSystemFactory(getInterface("VEngineModel0"), NULL));
			/*source->gTrace = */reinterpret_cast<void*>(appSystemFactory(getInterface("EngineTraceClient0"), NULL));
			/*source->gModelInfo = */reinterpret_cast<IVModelInfoClient*>(appSystemFactory(getInterface("VModelInfoClient0"), NULL));
			/*source->gSurface = */reinterpret_cast<vgui::ISurface*>(appSystemFactory(getInterface("VGUI_Surface0"), NULL));
			/*source->gMatSystem = */reinterpret_cast<IMaterialSystem*>(appSystemFactory(getInterface("VMaterialSystem0"), NULL));
			//DWORD* clientVmt = *(DWORD**)(source->gClient);
			//source->gInput = (CInput*)**(DWORD**)(clientVmt[14] + 0x2); //InActivateMouse();

			//lua.setDrawInstance(source->render);

			//source->nvar = new netvar(source->gClient);

			finishInit(root);
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
		void* appSystemFactory(const char* name, void* retcode)
		{
			return 0;
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
				Sleep( (DWORD)10 );
			}
			return fn;
		}

		CreateInterfaceFn fnEngine, fnClient, fnVstdlib, fnMatSurface, fnVgui2;//, appSystemFactory;

		static ioncsgo* instance;
	};
	ioncsgo* ioncsgo::instance;
}