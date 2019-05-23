////////////////////////////////////////////////////////////////////////////
//	Created 	: 30.09.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#define	_WIN32_WINNT	0x0500
#include <xray/os_include.h>
#include <xray/memory_crt_allocator.h>
#include "memory_process_allocator.h"
#include <errno.h>

xray::memory::crt_allocator_type*	xray::memory::g_crt_allocator = 0;

using xray::memory::crt_allocator;
using xray::memory::process_allocator;

static void	mem_usage		(HANDLE heap_handle, size_t* pAllocated, size_t* pTotalSize, u32* pBlocksUsed, u32* pBlocksFree)
{
	if ( !xray::debug::is_debugger_present( ) ) {
		if (pAllocated) *pAllocated = 0;
		if (pTotalSize) *pTotalSize = 0;
		if (pBlocksUsed) *pBlocksUsed = 0;
		if (pBlocksFree) *pBlocksFree = 0;

		return;
	}

	R_ASSERT(HeapValidate(heap_handle, 0, NULL), "Memory corruption");

	size_t	allocated	= 0;
	size_t	free		= 0;
	u32	blocks_free		= 0;
	u32	blocks_used		= 0;

	PROCESS_HEAP_ENTRY entry;
	entry.lpData	= NULL;

	HeapLock		( heap_handle );

	while ( HeapWalk( heap_handle, &entry ) ) {
		if ( entry.wFlags & PROCESS_HEAP_ENTRY_BUSY ) {
			blocks_used ++;
			allocated += entry.cbData;
		}
		else if ( entry.wFlags & PROCESS_HEAP_UNCOMMITTED_RANGE ) {
			blocks_free ++;
			free += entry.cbData;
		}
	}

	R_ASSERT(GetLastError() == ERROR_NO_MORE_ITEMS, "Memory corruption");

	HeapUnlock		( heap_handle );

	if (pBlocksFree)	*pBlocksFree = blocks_free;
	if (pBlocksUsed)	*pBlocksUsed = blocks_used;
	if (pAllocated)		*pAllocated  = allocated;
	if (pTotalSize)		*pTotalSize  = free + allocated;
}

crt_allocator::crt_allocator		( ) :
	m_malloc_ptr					( 0 ),
	m_free_ptr						( 0 ),
	m_realloc_ptr					( 0 )
{
/*
#ifdef _DLL
#	if _MSC_VER == 1500
		pcstr const library_name	= "msvcr90.dll";
#	elif _MSC_VER == 1912
		pcstr const library_name	= "msvcrt.dll";
#	else // #if _MSC_VER == 1500
#		error define correct library name here
#	endif // #if _MSC_VER == 1500

	HMODULE const handle			= GetModuleHandle( library_name );
	R_ASSERT						( handle );

	m_malloc_ptr					= (malloc_ptr_type)( GetProcAddress(handle, "malloc") );
	R_ASSERT						( m_malloc_ptr );

	m_free_ptr						= (free_ptr_type)( GetProcAddress(handle, "free") );
	R_ASSERT						( m_free_ptr );

	m_realloc_ptr					= (realloc_ptr_type)( GetProcAddress(handle, "realloc") );
	R_ASSERT						( m_realloc_ptr );
#endif // #ifdef _DLL
*/
	m_free_ptr = &free;
	m_malloc_ptr = &malloc;
	m_realloc_ptr = &realloc;
}

size_t crt_allocator::total_size		( ) const
{
	size_t total_size;
	mem_usage((HANDLE)_get_heap_handle(),0,&total_size,0,0);
	return total_size;
}

size_t crt_allocator::allocated_size	( ) const
{
	size_t allocated;
	mem_usage((HANDLE)_get_heap_handle(),&allocated,0,0,0);
	return allocated;
}

size_t process_allocator::total_size	( ) const
{
	if ( !memory::try_lock_process_heap	( ) )
		return					( 0 );

	size_t total_size;
	mem_usage					( GetProcessHeap(), 0, &total_size, 0, 0 );
	memory::unlock_process_heap	( );
	return						total_size;
}

size_t process_allocator::allocated_size( ) const
{
	if ( !memory::try_lock_process_heap	( ) )
		return					( 0 );

	size_t allocated;
	mem_usage					( GetProcessHeap(), &allocated, 0, 0, 0 );
	memory::unlock_process_heap	( );
	return						allocated;
}