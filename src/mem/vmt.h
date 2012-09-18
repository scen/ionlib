#pragma once

#include "../required.h"
#include "common.h"

namespace ion
{
	class vmt
	{
	public:
		// Find the number of vfuncs in a vtable
		static UINT countFuncs( void** pVMT );
		static UINT countFuncs( void* begin, void* end, void** pVMT );

		// Find the index for a vfunc, result is negative if not found
		static int findFunc( void** pVMT, void* pFunc, UINT vfuncs = 0 );
	protected:
		vmt( const vmt& );
		vmt& operator= ( const vmt& );
	public:
		vmt( void* inst, size_t offset = 0, UINT vfuncs = 0 );
		~vmt();
		vmt() {}

		inline void hookMethod( void* newfunc, size_t index )
		{
			_array[index+3] = newfunc;
		}

		inline void unhookMethod( size_t index )
		{
			_array[index+3] = _oldvmt[index];
		}

		// Manage the hooks.
		inline void unhook() {*_vftable = _oldvmt; }
		inline void rehook() { *_vftable = _array + 3; }
		inline bool hooked() const { return *_vftable!=_oldvmt; }
		inline void eraseHooks() { for ( UINT i = 0; i<_vcount; ++i ) _array[i+3] = _vftable[i]; }
		inline UINT numFuncs() const { return _vcount; }

		inline void poof() { _vftable = NULL; }

		template< typename Fn >
		inline Fn getMethod( size_t index ) const
		{
			return (Fn) _oldvmt[ index ];
		}

	protected:
		inline void _set_guard( size_t S ) { _array[1] = (void*) S; }
		inline size_t _get_guard() const { return (size_t) _array[1]; }
		inline void _set_backptr( void* ptr ) { _array[0] = ptr; }
		inline void* _get_backptr() const { return _array[0]; }

	private:
		void***	_vftable;
		void**	_oldvmt;
		void**	_array;
		UINT	_vcount;
	};
}