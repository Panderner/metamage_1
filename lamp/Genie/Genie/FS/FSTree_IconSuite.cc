/*	===================
 *	FSTree_IconSuite.cc
 *	===================
 */

#include "Genie/FS/FSTree_IconSuite.hh"

// POSIX
#include <sys/stat.h>

// vfs
#include "vfs/node.hh"
#include "vfs/methods/file_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/primitives/attach.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	static void iconsuite_copyfile( const vfs::node* that, const vfs::node* target );
	
	static const vfs::file_method_set iconsuite_file_methods =
	{
		NULL,
		&iconsuite_copyfile
	};
	
	static const vfs::node_method_set iconsuite_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		&iconsuite_file_methods
	};
	
	
	static void dispose_iconsuite( const vfs::node* that )
	{
		::IconSuiteRef& extra = *(::IconSuiteRef*) that->extra();
		
		const bool disposeData = true;
		
		(void) ::DisposeIconSuite( extra, disposeData );
	}
	
	
	static N::IconSuiteRef gStoredIconSuite;
	
	struct stored_IconSuite_scope
	{
		stored_IconSuite_scope( N::IconSuiteRef icon )
		{
			gStoredIconSuite = icon;
		}
		
		~stored_IconSuite_scope()
		{
			gStoredIconSuite = NULL;
		}
	};
	
	static void iconsuite_copyfile( const vfs::node* that, const vfs::node* target )
	{
		::IconSuiteRef extra = *(::IconSuiteRef*) that->extra();
		
		stored_IconSuite_scope scope( extra );
		
		attach( *target, *that );
	}
	
	
	vfs::node_ptr
	//
	New_FSTree_IconSuite( const vfs::node*             parent,
			              const plus::string&          name,
			              n::owned< N::IconSuiteRef >  iconSuite )
	{
		vfs::node* result = new vfs::node( parent,
		                                   name,
		                                   S_IFREG | 0400,
		                                   &iconsuite_methods,
		                                   sizeof (::IconSuiteRef),
		                                   &dispose_iconsuite );
		
		::IconSuiteRef& extra = *(::IconSuiteRef*) result->extra();
		
		extra = iconSuite.release();
		
		return result;
	}
	
	const N::IconSuiteRef Fetch_IconSuite()
	{
		return gStoredIconSuite;
	}
	
}
