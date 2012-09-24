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
	* client.dylib
		* `CHLClient`
			* After HudText()
				* Add `virtual void ShouldDrawDropdownConsole(void) = 0;`
			* After connect()
				* add `virtual int				Disconnect( void) = 0;`
		* `CInput`
			* added a bunch of joystick functions
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

* New CUserCmd crap
	* since csgo refuses to work with olly, to get addy of getusercmd in ida, getvfunc(ginput, 8) then find address of it then sub from base address then add to ida's base address (0x1000000)
	* http://www.gamedeception.net/threads/18658-L4D2-Verifying-CUserCmd
	* GetUserCmd takes 2 args -1, 0
			int __thiscall CInput__GetUserCmd(void *this, int nSlot, signed int sequence_number)
			{
			  int v3; // esi@2
			  int v5; // eax@4

			  if ( nSlot == -1 )
			    v3 = (int)((char *)this + 56);
			  else
			    v3 = (int)(this + 212 * nSlot + 56);
			  v5 = *(_DWORD *)(v3 + 172) + 100 * sequence_number % 150;// MULTIPLAYER_BACKUP = 150
			  return ((*(_DWORD *)(v5 + 4) != sequence_number) - 1) & v5;
			}

			.text:102480C0 ; int __thiscall CInput__GetUserCmd(void *this, int nSlot, signed int sequence_number)
			.text:102480C0 CInput__GetUserCmd proc near            ; DATA XREF: .rdata:105F2C74o
			.text:102480C0                                         ; .rdata:1061F4B4o
			.text:102480C0
			.text:102480C0 nSlot           = dword ptr  8
			.text:102480C0 sequence_number = dword ptr  0Ch
			.text:102480C0
			.text:102480C0                 push    ebp
			.text:102480C1                 mov     ebp, esp
			.text:102480C3                 mov     eax, [ebp+nSlot]
			.text:102480C6                 push    esi
			.text:102480C7                 cmp     eax, 0FFFFFFFFh ; CMP -1
			.text:102480CA                 jnz     short loc_102480D1
			.text:102480CC                 lea     esi, [ecx+38h]
			.text:102480CF                 jmp     short loc_102480DB
			.text:102480D1 ; ---------------------------------------------------------------------------
			.text:102480D1
			.text:102480D1 loc_102480D1:                           ; CODE XREF: CInput__GetUserCmd+Aj
			.text:102480D1                 imul    eax, 0D4h
			.text:102480D7                 lea     esi, [eax+ecx+38h]
			.text:102480DB
			.text:102480DB loc_102480DB:                           ; CODE XREF: CInput__GetUserCmd+Fj
			.text:102480DB                 mov     ecx, [ebp+sequence_number]
			.text:102480DE                 mov     eax, 1B4E81B5h
			.text:102480E3                 imul    ecx
			.text:102480E5                 sar     edx, 4
			.text:102480E8                 mov     eax, edx
			.text:102480EA                 shr     eax, 1Fh
			.text:102480ED                 add     edx, eax
			.text:102480EF                 imul    edx, 96h        ; MULTIPLAYER_BACKUP = 150 decimal
			.text:102480F5                 mov     eax, ecx
			.text:102480F7                 sub     eax, edx
			.text:102480F9                 imul    eax, 64h        ; sizeof(CUserCmd) = 100
			.text:102480FC                 add     eax, [esi+0ACh]
			.text:10248102                 xor     edx, edx
			.text:10248104                 cmp     [eax+4], ecx
			.text:10248107                 pop     esi
			.text:10248108                 setnz   dl
			.text:1024810B                 dec     edx
			.text:1024810C                 and     eax, edx
			.text:1024810E                 pop     ebp
			.text:1024810F                 retn    8
			.text:1024810F CInput__GetUserCmd endp
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
