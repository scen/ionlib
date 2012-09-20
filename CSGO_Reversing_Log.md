CSGO Reversing Log
==================

Here I'll be documenting the changes I'm making to CSGO and my reversing notes through it. Hopefully someone will find this useful, but I know I will when I need to update it.

Mac Bins
--------
* How to get Mac binaries
	* CSGO dedicated server -> Download app ID 740
	* All `.dylibs` should be there

* Updating classes
	* engine.dylib
		* `CEngineClient`
			* After `Key_BindingForKey()`
				* Add `virtual void Key_SetBinding(ButtonCode_t, char const*) = 0;`
	* vguimatsurface.dylib
		* CMatSystemSurface
			* After DrawPolyLine()
				* Add
					* virtual void DrawSetApparentDepth(float f) = 0;
					* virtual void DrawClearApparentDepth(float f) = 0;
			* After DrawSetTexture()
				* Add
					* virtual void DeleteTextureByID(int id) = 0;
					* SetFontGlyphSet function signature changed
	* TODO FINISH ADDING DrawTextBubble() etc etc

Reversing
---------

* IsDormant is no longer networked, it is a virtual of IClientNetworkable.
	* To get m_bIsDormant, reverse C_BaseEntity::IsDormant like so:
Code:
	char __cdecl C_BaseEntity__IsDormant(int a1)
	{
		char result; // al@2
	
		if ( *(_DWORD *)(a1 + 88) == -1 )
			result = 0;
		else
			result = *(_BYTE *)(a1 + 221) & 1;
		return result;	
	}

