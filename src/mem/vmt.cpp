#include "vmt.h"

namespace ion
{
	UINT vmt::countFuncs( void** vmt )
	{
		MEMORY_BASIC_INFORMATION mem;
		int i = -1;
		do { i++; VirtualQuery( vmt[i], &mem, sizeof(MEMORY_BASIC_INFORMATION) ); }
		while( mem.Protect==PAGE_EXECUTE_READ || mem.Protect==PAGE_EXECUTE_READWRITE );
		return i;
	}

	UINT vmt::countFuncs( void* begin, void* end, void** vmt )
	{
		int i = -1;
		do i++; while ( begin<vmt[i] && vmt[i]<end );
		return i;
	}

	int vmt::findFunc( void** vmt, void* func, UINT vfuncs )
	{
		if ( !vfuncs ) vfuncs = countFuncs( vmt );
		for ( UINT i = 0; i<vfuncs; i++ )
		{
			if ( vmt[i]==func ) return i;
		}
		return -1;
	}

	vmt::vmt( void* inst, size_t offset, UINT vfuncs )
	{
		_vftable = makeptr<void**>( inst, offset );
		_oldvmt = *_vftable;
		// Count vfuncs ourself if needed
		if ( !vfuncs ) vfuncs = countFuncs( _oldvmt );
		_vcount = vfuncs;
		// Allocate room for the new vtable
		_array = (void**)malloc( (vfuncs+3)*sizeof(void*) );
		// Initialize RTTI pointer
		_array[2] = _oldvmt[-1];	
		// Copy over the other vfuncs
		for ( UINT i = 0; i<vfuncs; ++i ) _array[i+3] = _oldvmt[i];
		// Hook it
		*_vftable = _array+3;
	}
	vmt::~vmt()
	{
		if ( _vftable ) unhook();
		free( _array );
	}
}