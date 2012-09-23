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

				float healthPct = (float)ent.getHealth() / 100.0;
				r->outlineRectOutline(rect(point(upperBB.x - bbWidth, upperBB.y), size(bbWidth * 2.0f, bbHeight)), col, color::black());
				r->fillRect(rect(point(upperBB.x + bbWidth + DistanceHP, upperBB.y), size(HPBarWidth, bbHeight)), color::black());
				r->fillRect(rect(point(upperBB.x + bbWidth + DistanceHP + 1, upperBB.y + 1 + ((bbHeight - 2.0f) * (1.0f - healthPct))), size(HPBarWidth - 2, bbHeight * healthPct - 2)), getHealthColor(healthPct));
				int x = upperBB.x + bbWidth + DistanceHP + HPBarWidth + DistanceText;
				int y = upperBB.y;
				size s = r->renderText(render::None, csgo->tahoma12, point(x, y), col, ent.getName());
				y = y + s.getHeight();
				s = r->renderText(render::None, csgo->tahoma12, point(x, y), col, format("%d HP") % ent.getHealth());
				y = y + s.getHeight();
				s = r->renderText(render::None, csgo->tahoma12, point(x, y), col, format("%.1f m") % (int)sqrt(ent.distanceTo()));
				y = y + s.getHeight();
				auto wep = weapon(ent.getActiveWeapon());
				auto own = wep.getPlayerOwner();
				s = r->renderText(render::None, csgo->tahoma12, point(x, y), col, format("wep: %s owner: 0x%X name=%s") % wep.getNiceName() % own % (!own?"69":entity(own).getName()));

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