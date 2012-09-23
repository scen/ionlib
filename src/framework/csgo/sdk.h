#pragma once

#define COMPILER_MSVC 1
#define COMPILER_MSVC32 1

#pragma region csgoincludes
//csgo sdk
#define private public //sue me
#define protected public
#include "../../sdk/hl2_csgo/public/tier0/wchartypes.h"

#include "../../sdk/hl2_csgo/public/cdll_int.h"
#include "../../sdk/hl2_csgo/public/iprediction.h"
#include "../../sdk/hl2_csgo/public/bone_setup.h"
#include "../../sdk/hl2_csgo/public/icliententitylist.h"
#include "../../sdk/hl2_csgo/public/tier1/checksum_md5.h"
#include "../../sdk/hl2_csgo/public/ienginevgui.h"
#include "../../sdk/hl2_csgo/public/IGameUIFuncs.h"
#include "../../sdk/hl2_csgo/public/dlight.h"
#include "../../sdk/hl2_csgo/public/iefx.h" 
#include "../../sdk/hl2_csgo/public/igameevents.h"
#include "../../sdk/hl2_csgo/public/view_shared.h"

#include "../../sdk/hl2_csgo/public/inetchannelinfo.h"

#include "../../sdk/hl2_csgo/game/client/imessagechars.h"
#include "../../sdk/hl2_csgo/game/client/iclientmode.h"
#include "../../sdk/hl2_csgo/game/client/cliententitylist.h"
#include "../../sdk/hl2_csgo/game/client/cdll_client_int.h"
#include "../../sdk/hl2_csgo/game/client/cbase.h"
#include "../../sdk/hl2_csgo/game/client/c_baseanimating.h"
#include "../../sdk/hl2_csgo/game/client/c_basecombatweapon.h"
#include "../../sdk/hl2_csgo/game/client/c_baseplayer.h"
#include "../../sdk/hl2_csgo/game/client/enginesprite.h"
#include "../../sdk/hl2_csgo/game/client/input.h"
#include "../../sdk/hl2_csgo/game/client/c_playerresource.h"
#include "../../sdk/hl2_csgo/game/client/iviewrender.h"
#include "../../sdk/hl2_csgo/game/client/viewrender.h"
#include "../../sdk/hl2_csgo/game/client/game_controls/commandmenu.h"
#include "../../sdk/hl2_csgo/game/client/hudelement.h"
#include "../../sdk/hl2_csgo/public/icvar.h"
//#include "../../sdk/hl2_csgo/game/shared/achievementmgr.h"
#include "../../sdk/hl2_csgo/public/tier1/convar.h"

#include "../../sdk/hl2_csgo/public/engine/ivmodelrender.h"
#include "../../sdk/hl2_csgo/public/engine/ivdebugoverlay.h"
#include "../../sdk/hl2_csgo/public/engine/ivmodelinfo.h"
#include "../../sdk/hl2_csgo/public/engine/IEngineTrace.h"
#include "../../sdk/hl2_csgo/public/engine/IEngineSound.h"

#include "../../sdk/hl2_csgo/public/materialsystem/imaterialsystemstub.h"
#include "../../sdk/hl2_csgo/public/materialsystem/itexture.h"
#include "../../sdk/hl2_csgo/public/materialsystem/IMaterialVar.h"

#include "../../sdk/hl2_csgo/public/matsys_controls/matsyscontrols.h"
#include "../../sdk/hl2_csgo/public/vgui/IClientPanel.h"
#include "../../sdk/hl2_csgo/public/vgui/IPanel.h"
#include "../../sdk/hl2_csgo/public/vgui/ISurfaceV30.h"
#include "../../sdk/hl2_csgo/public/vgui/ILocalize.h"
#undef private
#undef protected

#pragma comment(lib, "../src/sdk/hl2_csgo/lib/public/vstdlib.lib" )
#pragma comment(lib, "../src/sdk/hl2_csgo/lib/public/tier0.lib" )
#pragma comment(lib, "../src/sdk/hl2_csgo/lib/public/tier1.lib" )
#pragma comment(lib, "../src/sdk/hl2_csgo/lib/public/tier2.lib" )
#pragma comment(lib, "../src/sdk/hl2_csgo/lib/public/tier3.lib" )
#pragma comment(lib, "../src/sdk/hl2_csgo/lib/public/vgui_controls.lib" )
#pragma comment(lib, "../src/sdk/hl2_csgo/lib/public/mathlib.lib" )
#pragma comment(lib, "../src/sdk/hl2_csgo/lib/public/vstdlib.lib" )

#define IN_ATTACK					(1 << 0)
#define IN_JUMP						(1 << 1)
#define IN_DUCK						(1 << 2)
#define IN_FORWARD					(1 << 3)
#define IN_BACK						(1 << 4)
#define IN_USE						(1 << 5)
#define IN_CANCEL					(1 << 6)
#define IN_LEFT						(1 << 7)
#define IN_RIGHT					(1 << 8)
#define IN_MOVELEFT					(1 << 9)
#define IN_MOVERIGHT				(1 << 10)
#define IN_ATTACK2					(1 << 11)
#define IN_RUN						(1 << 12)
#define IN_RELOAD					(1 << 13)
#define IN_ALT1						(1 << 14)
#define IN_ALT2						(1 << 15)
#define IN_SCORE					(1 << 16)
#define IN_SPEED					(1 << 17)
#define IN_WALK						(1 << 18)
#define IN_ZOOM						(1 << 19)
#define IN_WEAPON1					(1 << 20)
#define IN_WEAPON2					(1 << 21)
#define IN_BULLRUSH					(1 << 22)

#pragma endregion csgoincludes