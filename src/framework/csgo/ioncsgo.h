#pragma once

#define CLIENT_DLL

//framework for csgo/swarm engine

#include "../../required.h"
#include "../../ionbase.h"
#include "../../mem/vmt.h"

#include "sdk.h"
#include "csgohook.h"
#include "interfaces.h"
#include "entity.h"
#include "vector.h"


namespace ion
{
	interfaces* csgo;
	class ioncsgo : public ionbase
	{
	public:
		ioncsgo(const std::string& proj)
		{
			csgo = new interfaces();
			instance = this;
			auto root = superInit(proj);

			if (root.isNull()) return;

			csgo->modClient = module("client.dll");
			csgo->modEngine = module("engine.dll");

			fnClient = captureFactory("client.dll");
			fnEngine = captureFactory("engine.dll");
			fnVstdlib = captureFactory("vstdlib.dll");
			fnMatSurface = captureFactory("vguimatsurface.dll");
			fnVgui2 = captureFactory("vgui2.dll");

			appSystemFactory = **(CreateInterfaceFn**)sigs["AppSystemFactory"];
			csgo->gGlobalVars = *(CGlobalVarsBase**)sigs["CGlobalVarsBase"];
			csgo->weaponIDAsString = sigs["WeaponIDAsString"];

			csgo->gEngine = reinterpret_cast<IVEngineClient*>(appSystemFactory(getInterface("VEngineClient0"), NULL));
			csgo->gClient = reinterpret_cast<IBaseClientDLL*>(fnClient(getInterface("VClient0"), NULL));
			csgo->gCvar = reinterpret_cast<ICvar*>(appSystemFactory(getInterface("VEngineCvar0"), NULL));
			csgo->gPanel = reinterpret_cast<vgui::IPanel*>(appSystemFactory(getInterface("VGUI_Panel0"), NULL));
			csgo->gEnt = reinterpret_cast<IClientEntityList*>(fnClient(getInterface("VClientEntityList0"), NULL));
			csgo->gModelRender = reinterpret_cast<IVModelRender*>(appSystemFactory(getInterface("VEngineModel0"), NULL));
			csgo->gTrace = reinterpret_cast<IEngineTrace*>(appSystemFactory(getInterface("EngineTraceClient0"), NULL));
			csgo->gModelInfo = reinterpret_cast<IVModelInfoClient*>(appSystemFactory(getInterface("VModelInfoClient0"), NULL));
			csgo->gSurface = reinterpret_cast<SurfaceV30::ISurface*>(appSystemFactory(getInterface("VGUI_Surface0"), NULL));
			csgo->gMatSystem = reinterpret_cast<IMaterialSystem*>(appSystemFactory(getInterface("VMaterialSystem0"), NULL));
			csgo->gDebugOverlay = reinterpret_cast<IVDebugOverlay*>(fnEngine(getInterface("VDebugOverlay0"), NULL));

			DWORD* clientVmt = *(DWORD**)(csgo->gClient);
			csgo->gInput = (CInput*)**(DWORD**)(clientVmt[15] + 0x2); //InActivateMouse();

			log.write(log.WARN, format("input = 0x%X\n") % csgo->gInput);

			//init fonts

			csgo->render = new csgorender(csgo->gSurface);
			lua.setDrawInstance(csgo->render);
			
			csgo->tahoma12 = (csgofont*)csgo->render->createFont("Tahoma", 12, render::Outline, 500);

			csgo->nvar = new netvar(csgo->gClient);

			csgo->clientHk = new vmt(csgo->gClient);
			csgo->clientHk->hookMethod(&csgohook::hkCreateMove, csgohook::CLIENT_CREATEMOVE);

			csgo->modelRenderHk = new vmt(csgo->gModelRender);
			csgo->modelRenderHk->hookMethod(&csgohook::hkDrawModelExecute, csgohook::MODELRENDER_DRAWMODELEXECUTE);
			
			csgo->panelHk = new vmt(csgo->gPanel);
			csgo->panelHk->hookMethod(&csgohook::hkPaintTraverse, csgohook::PANEL_PAINTTRAVERSE);

			//bind entity
			lua.registerScope(
				luabind::class_<entity>("entity")
					.def(luabind::constructor<int>())
					.def("isValid", &entity::isValid)
					.def("isAlive", &entity::isAlive)
					.def("getName", &entity::getName)
					.def("getType", &entity::getType)
					.def("getBonePos", &entity::getBonePos)
					.def("getClientClassName", &entity::getClientClassName)
					.def("getTeam", &entity::getTeam)
					.def("getHealth", &entity::getHealth)
					.def("isBot", &entity::isBot)
					.def("isDormant", &entity::isDormant)
					.def("getOrigin", &entity::getOrigin)
					.def("getFlags", &entity::getFlags)
					.def("isVisible", &entity::isVisible)
					.scope
					[
						luabind::def("me", &entity::me),
						luabind::def("getHighestEntityIndex", &entity::getHighestEntityIndex)
					]
					.def(luabind::const_self == luabind::other<entity>())
				);

			//bind vector
			lua.registerScope(
					luabind::class_<vector>("vector")
						.def(luabind::constructor<float, float, float>())
						.def(luabind::constructor<vector&>())
						.def(luabind::constructor<>())
						.def("toScreen", &vector::toScreen)
						.def_readwrite("x", &vector::x)
						.def_readwrite("y", &vector::y)
						.def_readwrite("z", &vector::z)
						.def_readwrite("visible", &vector::visible)
						.def(luabind::const_self == luabind::other<vector>())
						.def(luabind::const_self + luabind::other<vector>())
						.def(luabind::const_self / float())
					);

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
			for ( int i = 0; i < csgo->modClient.getLen(); i ++ )
				if ( bTextCompare( reinterpret_cast<PBYTE>(csgo->modClient.getStart() + i), pName ) )
				{
					log.write(log.VERB, format("%s\n")%std::string((char*)csgo->modClient.getStart() + i));
					return reinterpret_cast<char*>(csgo->modClient.getStart() + i);
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
				Sleep( (DWORD)10 );
			}
			return fn;
		}

		CreateInterfaceFn fnEngine, fnClient, fnVstdlib, fnMatSurface, fnVgui2, appSystemFactory;

		static ioncsgo* instance;
	};
	ioncsgo* ioncsgo::instance;
}