/*	=======
 *	time.cc
 *	=======
 */

// Standard C
#include <errno.h>

// POSIX
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/functions/execvp.hh"
#include "POSeven/functions/vfork.hh"
#include "POSeven/functions/wait.hh"
#include "POSeven/functions/_exit.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	int Main( int argc, iota::argv_t argv )
	{
		struct timeval tv_a = { 0 };
		struct timeval tv_b = { 0 };
		
		struct tms tms_a = { 0 };
		struct tms tms_b = { 0 };
		
		p7::throw_posix_result( ::times( &tms_a ) );
		
		p7::throw_posix_result( ::gettimeofday( &tv_a, NULL ) );
		
		p7::pid_t pid = POSEVEN_VFORK();
		
		if ( pid == 0 )
		{
			if ( argc <= 1 )
			{
				p7::_exit( p7::exit_success );
			}
			
			++argv;
			
			p7::execvp( argv[0], argv );
		}
		
		p7::wait_t wait_status = p7::wait();
		
		p7::throw_posix_result( ::gettimeofday( &tv_b, NULL ) );
		
		p7::throw_posix_result( ::times( &tms_b ) );
		
		unsigned long long a = tv_a.tv_sec * 1000000 + tv_a.tv_usec;
		unsigned long long b = tv_b.tv_sec * 1000000 + tv_b.tv_usec;
		
		float real_diff = (b - a) / 1000000.0;
		float user_diff = (tms_b.tms_cutime - tms_a.tms_cutime) * 1.0 / CLOCKS_PER_SEC;
		float sys_diff  = (tms_b.tms_cstime - tms_a.tms_cstime) * 1.0 / CLOCKS_PER_SEC;
		
		std::fprintf( stderr,             "\n"
		                      "real %.2f" "\n"
		                      "user %.2f" "\n"
		                      "sys  %.2f" "\n",
		                      real_diff, user_diff, sys_diff );
		
		return NN::Convert< p7::exit_t >( wait_status );
	}

}

