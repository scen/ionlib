CSGO Reversing Log
==================

Here I'll be documenting the changes I'm making to CSGO and my reversing notes through it. Hopefully someone will find this useful, but I know I will when I need to update it.

Mac Bins
--------
* How to get Mac binaries
	* CSGO dedicated server -> Download app ID 740
	* All `.dylibs` should be there

* App System Factory
	* String `g_ClientDLL->Init`
	* `8B 15 ? ? ? ? 8B 40 08 83 C4 04 68 ? ? ? ? 52 FF D0 85 C0 75 0D` + `0x2`
* CGlobalVarsBase
	* Same string
	* `68 ? ? ? ? 52 FF D0 85 C0 75 0D 68 ? ? ? ? E8 ? ? ? ? 83 C4 04` + `0x1`

* Updating classes
	* engine.dylib
		* `CEngineClient`
			* After `Key_BindingForKey()`
				* Add `virtual void Key_SetBinding(ButtonCode_t, char const*) = 0;`
		* `IVModelInfoClient` -> `IVModelInfo`
			* Added virtual function `virtual bool					HasAnimBlockBeenPreloaded(studiohdr_t  const*,int) const = 0;`
				* After GetAnimBlock();
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

            char __cdecl C_BaseEntity__IsDormant(int a1)
            {
            	char result; // al@2
            
            	if ( *(_DWORD *)(a1 + 88) == -1 )
            		result = 0;
            	else
            		result = *(_BYTE *)(a1 + 221) & 1;
            	return result;	
            }

* CClientEntityList (client.dylib)
	* aka IClientEntityList in the headers
	* Update to the headers, see the vtable dump.
	* Press X to see xrefs

* Entity iteration
	* `i -> {1, gEnt->GetHighestEntityIndex()}`
		* gEnt->GetClientNetworkable(i)
			* ->GetClientClass()
				* ->GetName() for ClassName e.g. CCsPlayer
