#pragma once

#include "../required.h"

namespace ion
{
	//helper functions
	inline void**& getvtable( void* inst, size_t offset = 0 )
	{
		return *reinterpret_cast<void***>( (size_t)inst + offset );
	}

	inline const void** getvtable( const void* inst, size_t offset = 0 )
	{
		return *reinterpret_cast<const void***>( (size_t)inst + offset );
	}

	template< typename Fn >
	inline Fn getvfunc( const void* inst, size_t index, size_t offset = 0 )
	{
		return reinterpret_cast<Fn>( getvtable( inst, offset )[ index ] );
	}

	template< typename T > inline T make_ptr( void* ptr, int offset ) 
	{ 
		return reinterpret_cast<T>( (size_t)ptr + offset ); 
	}

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

		inline void HookMethod( void* newfunc, size_t index )
		{
			_array[index+3] = newfunc;
		}

		inline void UnhookMethod( size_t index )
		{
			_array[index+3] = _oldvmt[index];
		}

		// Manage the hooks.
		inline void Unhook() {*_vftable = _oldvmt; }
		inline void Rehook() { *_vftable = _array + 3; }
		inline bool Hooked() const { return *_vftable!=_oldvmt; }
		inline void EraseHooks() { for ( UINT i = 0; i<_vcount; ++i ) _array[i+3] = _vftable[i]; }
		inline UINT NumFuncs() const { return _vcount; }

		inline void Poof() { _vftable = NULL; }

		template< typename Fn >
		inline Fn GetMethod( size_t index ) const
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