/*
	Genie/FS/sys/mac/machine/id.hh
	------------------------------
*/

#ifndef GENIE_FS_SYS_MAC_MACHINE_ID_HH
#define GENIE_FS_SYS_MAC_MACHINE_ID_HH

// vfs
#include "vfs/property.hh"


namespace Genie
{
	
	struct sys_mac_machine_id : vfs::readonly_property
	{
		static const int fixed_size = 4;
		
		static void get( plus::var_string& result, const vfs::node* that, bool binary );
	};
	
}

#endif
