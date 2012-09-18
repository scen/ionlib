ion RE Library
==============

The purpose of ion RELib is to aid the development process by reducing boilerplate code.

This project is based off my old Voxation RE libraries (pretty much refactored in C++11), which is located [here](http://github.com/scen/Voxation).

I wanted to switch it up because I was unhappy with the clumsy nested namespaces and clunky inheritance. Progress will be pretty slow, because of school, programming contests and all that good stuff.

This library is built for Visual Studio 2012 (VS110)

Notes for future reference
--------------------------

* Refer to [this](http://code.google.com/p/v8/wiki/BuildingWithGYP) to build V8
    * Make sure to build using /MD (Multi Threaded DLL)
	* Make sure to ignore LIBCMT.lib in `preparser`
	* HandleScope will CRASH if done in a different thread, use v8::Locker. Intentional problem described [here](https://groups.google.com/forum/?fromgroups=#!topic/v8-users/FXpeTYuAqKI)
* How to build luabind
    * Build Lua using [this method](http://stackoverflow.com/questions/6321927/compiling-lua-5-2-alpha-under-vs2010)
    * Get it off of [here](github.com/scen/luabind), bjam it up
        * This forked version is updated to Lua 5.2
		* Compile with exceptions
        * `bjam toolset=msvc release link=static exception-handling=on asynch-exceptions=off extern-c-nothrow=on`
		* How to make exceptions caught
			* Compile ionlib and your project with `/EHs`, which means extern functions CAN throw.
    * Set `LUA_PATH`, `BOOST_BUILD_PATH`, `PATH`, `BOOST_ROOT` accordingly.
* Luabind tutorial
    * http://blog.nuclex-games.com/tutorials/cxx/luabind-introduction/
* Replacing Lua with LuaJIT
	* They should have the same API
	* Follow instructions [here](http://luajit.org/install.html)
	* Build with `mscvbuild.bat static`. Link with `lua51.lib`
		* Edit `msvcbuild.bat` compile flags, add `/EHsc` for C++ exceptions (**IMPORTANT**)
* Source SDK HL2 OB Updates
	* Interface Dump
		* [15:19:10][VERB]:	VEngineClient014
		* [15:19:10][VERB]:	VClient016
		* [15:19:10][VERB]:	VEngineCvar004
		* [15:19:10][VERB]:	VGUI_Panel009
		* [15:19:10][VERB]:	VClientEntityList003
		* [15:19:10][VERB]:	VEngineModel016
		* [15:19:10][VERB]:	EngineTraceClient003
		* [15:19:10][VERB]:	VModelInfoClient006
		* [15:19:10][VERB]:	VGUI_Surface030
		* [15:19:10][VERB]:	VMaterialSystem080
	* Added https://forums.alliedmods.net/showthread.php?p=1782597
	* `ISurface`
		* Added `virtual void DeleteTextureByID(int i) PURE` after `IsTextureIDValid`
	* `C_BaseEntity`
		* Added `Padding001` before `SetHealth`
	* `cdll_int.h` -> `IBaseClientDLL`
		* Added 2 pad functions after `Shutdown()`
		* Added 7 pad functions after `CanRecordDemo()` (replay update)
* Source SDK HL2 CSGO Updates
	* Interface Dump
		* [15:15:27][VERB]:	VEngineClient013
		* [15:15:27][VERB]:	VClient016
		* [15:15:27][VERB]:	VEngineCvar007
		* [15:15:27][VERB]:	VGUI_Panel009
		* [15:15:27][VERB]:	VClientEntityList003
		* [15:15:27][VERB]:	VEngineModel016
		* [15:15:27][VERB]:	EngineTraceClient004
		* [15:15:27][VERB]:	VModelInfoClient004
		* [15:15:27][VERB]:	VGUI_Surface031
		* [15:15:27][VERB]:	VMaterialSystem080



About
-----

ion RElib was programmed by Stanley Cen over the course of a few weeks, and is in constant, albeit slow, development.

This software is provided as-is, and is licensed under the MIT License. The license text is located under `LICENSE`, as well as below for your convenience.

    Copyright (c) 2012 Stanley Cen
    
    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:
    
    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
    LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
    OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.