/*
	kernel_boundary.cc
	------------------
*/

#include "Genie/Dispatch/kernel_boundary.hh"

// mac-sys-utils
#include "mac_sys/current_thread_stack_space.hh"

// relix-kernel
#include "relix/signal/call_signal_handler.hh"
#include "relix/signal/caught_signal.hh"
#include "relix/signal/deliver_fatal_signal.hh"
#include "relix/time/cpu_time_checkpoint.hh"

// Genie
#include "Genie/api/breathe.hh"


#ifndef SIGSTKFLT
#define SIGSTKFLT  (-1)
#endif


namespace Genie
{
	
	void enter_system_call( long syscall_number, long* params )
	{
		relix::enter_system();
		
		const unsigned long space = mac::sys::current_thread_stack_space();
		
		// space will be 0 if we're not on a Thread Manager stack
		
		if ( space != 0  &&  space < 8192 )
		{
			relix::deliver_fatal_signal( SIGSTKFLT );
		}
		
	rebreathe:
		
		try
		{
			breathe( true );
		}
		catch ( const relix::caught_signal& signal )
		{
			relix::leave_system();
			
			relix::call_signal_handler( signal );
			
			relix::enter_system();
			
			goto rebreathe;
		}
	}
	
	bool leave_system_call( int result )
	{
		relix::leave_system();
		
		if ( relix::the_caught_signal.signo )
		{
			const relix::caught_signal signal = relix::the_caught_signal;
			
			relix::the_caught_signal.signo = 0;
			
			relix::call_signal_handler( signal );
			
			return signal.action.sa_flags & SA_RESTART;
		}
		
		return false;
	}
	
}

