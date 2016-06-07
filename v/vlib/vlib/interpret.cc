/*
	interpret.cc
	------------
*/

#include "vlib/interpret.hh"

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>

// Standard C++
#include <new>

// must
#include "must/write.h"

// gear
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/var_string.hh"

// vlib
#include "vlib/exceptions.hh"
#include "vlib/execute.hh"
#include "vlib/parse.hh"
#include "vlib/string-utils.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define FAIL( s )  ::vlib::fail( STR_LEN( "ERROR: " s "\n" ) )


namespace vlib
{
	
	static
	void fail( const char* msg, unsigned len )
	{
		must_write( STDERR_FILENO, msg, len );
	
		exit( 1 );
	}
	
	static
	void fail( const plus::string& s, const source_spec& src )
	{
		plus::var_string msg;
		
		if ( src.file != NULL )
		{
			const char* line_num = gear::inscribe_unsigned_decimal( src.line );
			
			msg += src.file;
			msg += ":";
			msg += line_num;
			msg += "\n    ";
		}
		
		msg += s;
		msg += "\n";
		
		must_write( STDERR_FILENO, msg.data(), msg.size() );
		
		exit( 1 );
	}
	
	Value interpret( const char*     program,
	                 const char*     file,
	                 lexical_scope*  globals )
	{
		try
		{
			return execute( parse( program, file, globals ) );
		}
		catch ( const std::bad_alloc& )
		{
			FAIL( "Out of memory!" );
		}
		catch ( const transfer_via_break& e )
		{
			plus::string msg = "ERROR: `break` used outside of loop";
			
			fail( msg, e.source );
		}
		catch ( const transfer_via_continue& e )
		{
			plus::string msg = "ERROR: `continue` used outside of loop";
			
			fail( msg, e.source );
		}
		catch ( const language_error& e )
		{
			plus::var_string msg = "ERROR: ";
			
			msg += e.text;
			
			fail( msg, e.source );
		}
		catch ( const user_exception& e )
		{
			plus::var_string msg = "ERROR: uncaught exception: ";
			
			msg += rep( e.object );
			
			fail( msg, e.source );
		}
		catch ( const invalid_token_error& e )
		{
			plus::var_string msg = "ERROR: invalid token '";
			
			msg += e.token;
			msg += "'";
			
			fail( msg, e.source );
		}
		catch ( const undeclared_symbol_error& e )
		{
			plus::var_string msg = "ERROR: undeclared symbol '";
			
			msg += e.name;
			msg += "'";
			
			fail( msg, e.source );
		}
		
		return Value();
	}
	
}
