/*
	Genie/FS/sys/mac/thread/default-stack-size.hh
	---------------------------------------------
*/

#ifndef GENIE_FS_SYS_MAC_THREAD_DEFAULTSTACKSIZE_HH
#define GENIE_FS_SYS_MAC_THREAD_DEFAULTSTACKSIZE_HH

// vfs
#include "vfs/property.hh"


namespace Genie
{
	
	struct sys_mac_thread_defaultstacksize : vfs::readonly_property
	{
		static const int fixed_size = 4;
		
		static void get( plus::var_string& result, const vfs::node* that, bool binary );
	};
	
}

#endif
