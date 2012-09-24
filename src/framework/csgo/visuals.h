#pragma once

#include "sdk.h"
#include "interfaces.h"
#include "entity.h"
#include "weapon.h"
#include "../../render/csgo/csgorender.h"

namespace ion
{
	extern interfaces* csgo;
	class visuals
	{
	public:
		const static color enemyHidden, enemyVisible, teamHidden, teamVisible, healthGreen, healthYellow, healthRed;

		const static color& getHealthColor(float pct)
		{
			if (pct > 0.60f) return healthGreen;
			if (pct > 0.30f) return healthYellow;
			return healthRed;
		}

		static void renderC4(entity& ent)
		{
			auto wep = weapon(ent.baseEnt);
			auto holder = wep.getPlayerOwner();
			auto bone = holder.isAlive() ? holder.getBonePos(holder.MID_CHEST).toScreen() : wep.getBonePos(0).toScreen();

			static color c4(252, 88, 209, 255);

			if (bone.visible)
			{
				auto r = csgo->render;
				const static int TextSpace = 3;
				r->renderCornerRect(rect(point(bone.x - 8, bone.y - 8), size(16, 16)), c4);
				r->renderText(0, csgo->tahoma12, point(bone.x + 8 + TextSpace, bone.y - 8), c4, "C4");
				r->renderText(render::RAlign, csgo->tahoma12, point(bone.x - 8 - TextSpace, bone.y - 8), c4, format("%d m") % (int)ent.distanceTo());
			}
		}

		static void renderWeapon(entity& ent)
		{
			static color weaponCol(214, 181, 90, 255);
			auto wep = weapon(ent.baseEnt);
			if (wep.getPlayerOwner().isValid()) return;
			auto bone = wep.getBonePos(0).toScreen();
			if (bone.visible)
			{
				auto r = csgo->render;
				const static int TextSpace = 3;
				r->renderCornerRect(rect(point(bone.x - 8, bone.y - 8), size(16, 16)), weaponCol);
				r->renderText(0, csgo->tahoma12, point(bone.x + 8 + TextSpace, bone.y - 8), weaponCol, std::string(wep.getNiceName()));
				r->renderText(render::RAlign, csgo->tahoma12, point(bone.x - 8 - TextSpace, bone.y - 8), weaponCol, format("%d m") % (int)ent.distanceTo());
			}
		}

		static void renderPlantedC4(entity& ent)
		{
			auto bone = ent.getBonePos(0).toScreen();

			static color c4planted(252, 88, 209, 255);

			if (bone.visible)
			{
				auto r = csgo->render;
				auto base = ent.baseEnt;
				const static int TextSpace = 3;
				r->renderCornerRect(rect(point(bone.x - 8, bone.y - 8), size(16, 16)), c4planted);
				r->renderText(0, csgo->tahoma12, point(bone.x + 8 + TextSpace, bone.y - 8), c4planted, "C4 (Live)");
				r->renderText(render::RAlign, csgo->tahoma12, point(bone.x - 8 - TextSpace, bone.y - 8), c4planted, format("%d m") % (int)ent.distanceTo());
				r->renderText(render::Center, csgo->tahoma12, point(bone.x, bone.y + 8 + TextSpace), c4planted, format("%.1f s") % ( *makeptr<float>(base, csgo->nvar->c4_Blow) -csgo->gGlobalVars->curtime ));
				/*	% *makeptr<bool>(base, csgo->nvar->c4_Ticking) % *makeptr<float>(base, csgo->nvar->c4_Blow)
					% *makeptr<float>(base, csgo->nvar->c4_TimerLen)
					% *makeptr<float>(base, csgo->nvar->c4_DefuseLen)
					% *makeptr<float>(base, csgo->nvar->c4_DefuseCountDown));*/
			}
		}

		static void renderPlayer(entity& ent)
		{
			player_info_s pinfo;
			if (!ent.isAlive() || !csgo->gEngine->GetPlayerInfo(ent.idx, &pinfo)) return;
			if (ent.getOrigin().toScreen().visible)
			{
				auto me = entity::me();
				if (!me.baseEnt) return;
				auto r = csgo->render;
				bool crouch = (ent.getFlags() & IN_DUCK);
				vector head = ent.getBonePos(ent.HEAD);
				vector origHead = head.toScreen();

				if (!crouch) head.z += 10;
				vector upperBB = head.toScreen();
				if (!crouch) head.z -= 80;
				else head.z -= 35;

				vector lowerBB = head.toScreen();

				auto bbHeight = fabs(upperBB.y - lowerBB.y);
				auto bbWidth = bbHeight * 0.25f;
				if (crouch) bbWidth = bbHeight / 2.0f;

				color col;
				if (ent.getTeam() == me.getTeam())
				{
					if (ent.isVisible(entity::HEAD))
					{
						col = teamVisible;
					}
					else
					{
						col = teamHidden;
					}
				}
				else
				{
					if (ent.isVisible(entity::HEAD))
					{
						col = enemyVisible;
					}
					else
					{
						col = enemyHidden;
					}
				}

				const static int DistanceHP = 2;
				const static int HPBarWidth = 5;
				const static int DistanceText = 2;
				const static int DistanceTextLeft = 3;

				float healthPct = (float)ent.getHealth() / 100.0;
				r->outlineRectOutline(rect(point(upperBB.x - bbWidth, upperBB.y), size(bbWidth * 2.0f, bbHeight)), col, color::black());
				r->fillRect(rect(point(upperBB.x + bbWidth + DistanceHP, upperBB.y), size(HPBarWidth, bbHeight)), color::black());
				r->fillRect(rect(point(upperBB.x + bbWidth + DistanceHP + 1, upperBB.y + 1 + ((bbHeight - 2.0f) * (1.0f - healthPct))), size(HPBarWidth - 2, (bbHeight-2.0f) * healthPct )), getHealthColor(healthPct));
				
				int lx = upperBB.x - bbWidth - DistanceTextLeft;
				int ly = upperBB.y;

				size ls;

				//right side
				int x = upperBB.x + bbWidth + DistanceHP + HPBarWidth + DistanceText;
				int y = upperBB.y;
				size s = r->renderText(render::None, csgo->tahoma12, point(x, y), col, ent.getName());
				y = y + s.getHeight();
				s = r->renderText(render::None, csgo->tahoma12, point(x, y), col, format("%d HP") % ent.getHealth());
				y = y + s.getHeight();
				s = r->renderText(render::None, csgo->tahoma12, point(x, y), col, format("%.1f m") % (int)ent.distanceTo());
				y = y + s.getHeight();
				auto wep = weapon(ent.getActiveWeapon());
				s = r->renderText(render::None, csgo->tahoma12, point(x, y), col, std::string(wep.getNiceName()));

				r->renderCornerRect(rect(point(origHead.x - 8, origHead.y - 8), size(16, 16)), col);
			}
		}
	};
	const color visuals::enemyHidden(222, 159, 95, 255);
	const color visuals::enemyVisible = color(247, 136, 136, 255);
	const color visuals::teamHidden = color(116, 182, 110, 255);
	const color visuals::teamVisible = color(136, 188, 247, 255);
	const color visuals::healthGreen = color(33, 219, 58, 255);
	const color visuals::healthYellow = color(200, 209, 23, 255);
	const color visuals::healthRed = color(245, 83, 83, 255);
};