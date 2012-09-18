#pragma once

#pragma region source07_includes
//source07 sdk
#define private public //sue me
#define protected public
#include "../../sdk/hl2_ob/public/tier0/wchartypes.h"

#include "../../sdk/hl2_ob/public/cdll_int.h"
#include "../../sdk/hl2_ob/public/iprediction.h"
#include "../../sdk/hl2_ob/public/bone_setup.h"
#include "../../sdk/hl2_ob/public/icliententitylist.h"
#include "../../sdk/hl2_ob/public/tier1/checksum_md5.h"
#include "../../sdk/hl2_ob/public/ienginevgui.h"
#include "../../sdk/hl2_ob/public/IGameUIFuncs.h"
#include "../../sdk/hl2_ob/public/dlight.h"
#include "../../sdk/hl2_ob/public/iefx.h" 
#include "../../sdk/hl2_ob/public/igameevents.h"
#include "../../sdk/hl2_ob/public/view_shared.h"

#include "../../sdk/hl2_ob/public/inetchannelinfo.h"

#include "../../sdk/hl2_ob/game/client/imessagechars.h"
#include "../../sdk/hl2_ob/game/client/iclientmode.h"
#include "../../sdk/hl2_ob/game/client/cliententitylist.h"
#include "../../sdk/hl2_ob/game/client/cdll_client_int.h"
#include "../../sdk/hl2_ob/game/client/cbase.h"
#include "../../sdk/hl2_ob/game/client/c_baseanimating.h"
#include "../../sdk/hl2_ob/game/client/c_basecombatweapon.h"
#include "../../sdk/hl2_ob/game/client/c_baseplayer.h"
#include "../../sdk/hl2_ob/game/client/enginesprite.h"
#include "../../sdk/hl2_ob/game/client/input.h"
#include "../../sdk/hl2_ob/game/client/c_playerresource.h"
#include "../../sdk/hl2_ob/game/client/hl2mp/c_hl2mp_player.h"
#include "../../sdk/hl2_ob/game/client/iviewrender.h"
#include "../../sdk/hl2_ob/game/client/viewrender.h"
#include "../../sdk/hl2_ob/game/client/game_controls/commandmenu.h"
#include "../../sdk/hl2_ob/game/client/hudelement.h"
#include "../../sdk/hl2_ob/public/icvar.h"
//#include "../../sdk/hl2_ob/game/shared/achievementmgr.h"
#include "../../sdk/hl2_ob/public/tier1/convar.h"

#include "../../sdk/hl2_ob/public/engine/ivmodelrender.h"
#include "../../sdk/hl2_ob/public/engine/ivdebugoverlay.h"
#include "../../sdk/hl2_ob/public/engine/ivmodelinfo.h"
#include "../../sdk/hl2_ob/public/engine/IEngineTrace.h"
#include "../../sdk/hl2_ob/public/engine/IEngineSound.h"

#include "../../sdk/hl2_ob/public/materialsystem/imaterialsystemstub.h"
#include "../../sdk/hl2_ob/public/materialsystem/itexture.h"
#include "../../sdk/hl2_ob/public/materialsystem/IMaterialVar.h"

#include "../../sdk/hl2_ob/public/matsys_controls/matsyscontrols.h"
#include "../../sdk/hl2_ob/public/vgui/IClientPanel.h"
#include "../../sdk/hl2_ob/public/vgui/IPanel.h"
#include "../../sdk/hl2_ob/public/vgui/ISurface.h"
#include "../../sdk/hl2_ob/public/vgui/ILocalize.h"
#undef private
#undef protected

#pragma comment(lib, "../src/sdk/hl2_ob/lib/public/vstdlib.lib" )
#pragma comment(lib, "../src/sdk/hl2_ob/lib/public/tier0.lib" )
#pragma comment(lib, "../src/sdk/hl2_ob/lib/public/tier1.lib" )
#pragma comment(lib, "../src/sdk/hl2_ob/lib/public/tier2.lib" )
#pragma comment(lib, "../src/sdk/hl2_ob/lib/public/tier3.lib" )
#pragma comment(lib, "../src/sdk/hl2_ob/lib/public/vgui_controls.lib" )
#pragma comment(lib, "../src/sdk/hl2_ob/lib/public/mathlib.lib" )
#pragma comment(lib, "../src/sdk/hl2_ob/lib/public/vstdlib.lib" )

#pragma endregion source07_includes