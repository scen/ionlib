#pragma once

//wrap the source vector class

#include "../../required.h"
#include "sdk.h"
#include "interfaces.h"

namespace ion
{
	extern interfaces* source;
	class vector : public Vector
	{
	public:

		//Yay, C++11 ctor inheritance
		using Vector::Vector;

		vector() : Vector() {}
		vector(vec_t x, vec_t y, vec_t z) : Vector(x, y, z) {}
		vector(Vector& v) : Vector(v) {}

		vector toScreen( )
		{
			vector ret;
			if( !screenTransform( *this, ret))
			{
				int ScreenWidth, ScreenHeight;
				source->gEngine->GetScreenSize( ScreenWidth, ScreenHeight );
				float x = ScreenWidth / 2;
				float y = ScreenHeight / 2;
				x += 0.5 * ret.x * ScreenWidth + 0.5;
				y -= 0.5 * ret.y * ScreenHeight + 0.5;
				ret.x = x;
				ret.y = y;
				ret.visible = true;
				return ret;
			}
			ret.visible = false;
			return ret;
		}
		
		bool visible;

	private:
		bool screenTransform( const vector &p, vector &s )
		{
			float w;
			const VMatrix &worldToScreen = source->gEngine->WorldToScreenMatrix();

			s.x = worldToScreen[0][0] * p[0] + worldToScreen[0][1] * p[1] + worldToScreen[0][2] * p[2] + worldToScreen[0][3];
			s.y = worldToScreen[1][0] * p[0] + worldToScreen[1][1] * p[1] + worldToScreen[1][2] * p[2] + worldToScreen[1][3];
			w	= worldToScreen[3][0] * p[0] + worldToScreen[3][1] * p[1] + worldToScreen[3][2] * p[2] + worldToScreen[3][3];
			s.z = 0.0f;

			bool behind = false;
			if( w < 0.001f )
			{
				behind = true;
				s.x *= 100000;
				s.y *= 100000;
			}
			else
			{
				behind = false;
				float invw = 1.0f / w;
				s.x *= invw;
				s.y *= invw;
			}

			return behind;
		}
	};
}