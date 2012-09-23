#pragma once

//wrap the csgo-source vector class

#include "../../required.h"
#include "sdk.h"
#include "interfaces.h"

namespace ion
{
	extern interfaces* csgo;
	class vector : public Vector
	{
	public:

		vector() : Vector() {}
		vector(vec_t x, vec_t y, vec_t z) : Vector(x, y, z) {}
		vector(Vector& v) : Vector(v) {}

		vector toScreen() const
		{
			vector ret;
			bool a = csgo->gDebugOverlay->ScreenPosition(*this, ret);
			ret.visible = !a;
			return ret;
		}
		
		bool visible;
		static const vector empty;
	private:
	};
	const vector vector::empty = vector();
}