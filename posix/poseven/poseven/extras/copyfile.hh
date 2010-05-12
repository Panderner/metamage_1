// copyfile.hh
// -----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_COPYFILE_HH
#define POSEVEN_FUNCTIONS_COPYFILE_HH

// compat
#include "lamp/copyfile.h"

// iota
#include "iota/strings.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	inline void copyfile( const char* from, const char* to )
	{
		throw_posix_result( ::copyfile( from, to ) );
	}
	
	template < class String1, class String2 >
	inline void copyfile( const String1& from, const String2& to )
	{
		copyfile( iota::get_string_c_str( from ), iota::get_string_c_str( to ) );
	}
	
}

#endif

