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


* CIVDebugOverlay has built in W2S
	* Dump in engine.dylib
	* Added to SDK CSGO
	* Can render spheres
* C_BaseEntity->index
	* Offset is 0x64 at the moment

* WeaponIDS
	* In mac bins the functions are called BLah::GetCSWeaponID() they just mov a # into eax
	* Make ida script to loop through all functions
		* then run ` if ( strstr(GetMnem(CurrentOperand), "mov") != -1 && strstr( GetOpnd( CurrentOperand, 0 ), "eax" ) != -1 && GetOperandValue( CurrentOperand, 1 ) == 33 )`
			* 33 signifies the weapon id ur searching for, make sure it's odd so u get a match
			* xrefs, vtable, difference
				* vtable start 106A493C 
				* Get CS weapon ID 106A4F2C
				* delta = 0x5F0 (380'th vfunc)

* Weapon getplayerowner
	* fx_firebullets in client.dylib, xrefs show the first func called is getplayer owner, match with client.dll
	* 396th vfunc

* FX_FireBullets (Client.dll)
	* Look in mac bin to find searchable string "old: %f"
	* Find how it is called in client.dll, and see hwo to get the weapon id
	* look @ how it gets called by using xrefs (x)
	* 4th arg is wepid
	* gets wepid using `sub_101E8960((void *)v3, (int)&v44);`
	* Find function WeaponIdAsString (currently 0x101D9450) (it just reads from an array)
            char *__cdecl WepIdAsString(int a1)
            {
              unsigned int v1; // eax@1
            
              v1 = 0;
              while ( dword_107CAC10[2 * v1] != a1 )
              {
                ++v1;
                if ( v1 >= 0x36 )
                  return 0;
              }
              return (char *)off_107CAC14[2 * v1];
            }

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
