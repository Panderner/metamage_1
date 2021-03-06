// Nitrogen/CodeFragments.hh
// -------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_CODEFRAGMENTS_HH
#define NITROGEN_CODEFRAGMENTS_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __CODEFRAGMENTS__
#include <CodeFragments.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_ERRORCODE_HH
#include "nucleus/error_code.hh"
#endif
#ifndef NUCLEUS_ERRORSREGISTERED_HH
#include "nucleus/errors_registered.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_CODEFRAGMENTS_TYPES_CFRAGCONNECTIONID_HH
#include "Mac/CodeFragments/Types/CFragConnectionID.hh"
#endif
#ifndef MAC_RESOURCES_TYPES_RESID_HH
#include "Mac/Resources/Types/ResID.hh"
#endif

#ifndef NITROGEN_STR_HH
#include "Nitrogen/Str.hh"
#endif

#ifdef CFragHasFileLocation
#undef CFragHasFileLocation

inline bool CFragHasFileLocation( CFragLocatorKind where )
{
	return where == kDataForkCFragLocator
		|| where == kResourceCFragLocator
	#if TARGET_RT_MAC_CFM
		|| where == kCFBundleIntCFragLocator
	#endif
		;
}

#endif

namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( CodeFragmentManager );
	
#if ! __LP64__
	
	static const Mac::ResID kCFragResourceID = Mac::ResID( ::kCFragResourceID );
	
	enum CFragArchitecture
	{
		kPowerPCCFragArch     = ::kPowerPCCFragArch,
		kMotorola68KCFragArch = ::kMotorola68KCFragArch,
		kAnyCFragArch         = ::kAnyCFragArch,
		
	#if TARGET_CPU_PPC || TARGET_CPU_68K
		
		kCompiledCFragArch    = ::kCompiledCFragArch,
		
	#endif
		
		kCFragArchitecture_Max = nucleus::enumeration_traits< ::CFragArchitecture >::max
	};
	
	enum CFragUsage
	{
		kImportLibraryCFrag   = ::kImportLibraryCFrag,
		kApplicationCFrag     = ::kApplicationCFrag,
		kDropInAdditionCFrag  = ::kDropInAdditionCFrag,
		kStubLibraryCFrag     = ::kStubLibraryCFrag,
		kWeakStubLibraryCFrag = ::kWeakStubLibraryCFrag,
		
		kCFragUsage_Max = nucleus::enumeration_traits< ::CFragUsage >::max
	};
	
	enum CFragLocatorKind
	{
		kMemoryCFragLocator        = ::kMemoryCFragLocator,
		kDataForkCFragLocator      = ::kDataForkCFragLocator,
		kResourceCFragLocator      = ::kResourceCFragLocator,
		kNamedFragmentCFragLocator = ::kNamedFragmentCFragLocator,
		kCFBundleCFragLocator      = ::kCFBundleCFragLocator,
		
	#if !TARGET_RT_MAC_MACHO
		
		kCFBundleIntCFragLocator   = ::kCFBundleIntCFragLocator,
		
	#endif
		
		kCFragLocatorKind_Max = nucleus::enumeration_traits< ::CFragLocatorKind >::max
	};
	
	enum CFragLoadOptions
	{
		kReferenceCFrag   = ::kReferenceCFrag,
		kFindCFrag        = ::kFindCFrag,
		kPrivateCFragCopy = ::kPrivateCFragCopy,
		
		kCFragLoadOptions_Max = nucleus::enumeration_traits< ::CFragLoadOptions >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( CFragLoadOptions )
	
	static const void* const kUnresolvedCFragSymbolAddress = NULL;
	
	enum CFragSymbolClass
	{
		kCodeCFragSymbol    = ::kCodeCFragSymbol,
		kDataCFragSymbol    = ::kDataCFragSymbol,
		kTVectorCFragSymbol = ::kTVectorCFragSymbol,
		kTOCCFragSymbol     = ::kTOCCFragSymbol,
		kGlueCFragSymbol    = ::kGlueCFragSymbol,
		
		kCFragSymbolClass_Max = nucleus::enumeration_traits< ::CFragSymbolClass >::max
	};
	
	// Opaque pointer type
	typedef struct SymbolAddress* SymbolAddressPtr;
	
	// errMessage wrapper
	struct ErrMessage
	{
		Str255 errMessage;
		
		ErrMessage( ConstStr255Param errMessage ) : errMessage( errMessage )
		{
		}
	};
	
	// OSStatus wrapper that carries errMessage
	template < class ErrorCode >
	struct OSStatusErrMessage : ErrorCode,
	                            ErrMessage
	{
		OSStatusErrMessage( ConstStr255Param errMessage )
		:
			ErrMessage( errMessage )
		{
		}
		
	#if !NUCLEUS_RICH_ERRORCODES
		
		OSStatusErrMessage( ::OSStatus err, ConstStr255Param errMessage )
		:
			ErrorCode ( err        ),
			ErrMessage( errMessage )
		{
		}
		
	#endif
	};
	
	template < CFragLoadOptions options >  struct CFragLoadOptions_Traits;
	
	struct Find_CFragConnection_Traits
	{
		typedef CFragConnectionID Result;
		
		static Result MakeResult( CFragConnectionID connID )  { return connID; }
	};
	
	struct Reference_CFragConnection_Traits
	{
		typedef nucleus::owned< CFragConnectionID > Result;
		
		static Result MakeResult( CFragConnectionID connID )
		{
			return nucleus::owned< CFragConnectionID >::seize( connID );
		}
	};
	
	template <>  struct CFragLoadOptions_Traits< kFindCFrag > : Find_CFragConnection_Traits {};
	
	template <>  struct CFragLoadOptions_Traits< kReferenceCFrag   > : Reference_CFragConnection_Traits {};
	template <>  struct CFragLoadOptions_Traits< kPrivateCFragCopy > : Reference_CFragConnection_Traits {};
	
	template < class SymbolAddressType >
	inline SymbolAddressType SymAddr_Cast( SymbolAddressPtr symAddr )
	{
		// The double reinterpret_cast is necessary if SymbolAddressType is a 
		// pointer-to-function.  Gcc doesn't like casting a pointer-to-object
		// to a pointer-to-function, so we use long as an intermediary.
		
		return reinterpret_cast< SymbolAddressType >( reinterpret_cast< long >( symAddr ) );
	}
	
	// GetSharedLibrary
	
	void GetSharedLibrary( ConstStr63Param     libName,
	                       CFragArchitecture   archType,
	                       CFragLoadOptions    options,
	                       CFragConnectionID*  connID   = NULL,
	                       SymbolAddressPtr*   mainAddr = NULL );
	
	template < CFragLoadOptions options, class MainAddrType >
	inline typename CFragLoadOptions_Traits< options >::Result
	//
	GetSharedLibrary( ConstStr63Param     libName,
	                  CFragArchitecture   archType,
	                  MainAddrType*       mainAddr )
	{
		typedef CFragLoadOptions_Traits< options > Traits;
		
		CFragConnectionID connID;
		SymbolAddressPtr tempMainAddr;
		
		GetSharedLibrary( libName,
		                  archType,
		                  CFragLoadOptions( options ),
		                  &connID,
		                  &tempMainAddr );
		
		if ( mainAddr != NULL )
		{
			*mainAddr = SymAddr_Cast< MainAddrType >( tempMainAddr );
		}
		
		return Traits::MakeResult( connID );
	}
	
	template < CFragLoadOptions options >
	inline typename CFragLoadOptions_Traits< options >::Result
	//
	GetSharedLibrary( ConstStr63Param     libName,
	                  CFragArchitecture   archType )
	{
		return GetSharedLibrary< options, SymbolAddressPtr >( libName,
		                                                      archType,
		                                                      NULL );
	}
	
	void GetDiskFragment( const FSSpec&       file,
	                      std::size_t         offset,
	                      std::size_t         length,
	                      ConstStr63Param     fragName,
	                      CFragLoadOptions    options,
	                      CFragConnectionID*  connID   = NULL,
	                      SymbolAddressPtr*   mainAddr = NULL );
	
	template < CFragLoadOptions options, class MainAddrType >
	inline typename CFragLoadOptions_Traits< options >::Result
	//
	GetDiskFragment( const FSSpec&    file,
	                 std::size_t      offset,
	                 std::size_t      length,
	                 ConstStr63Param  fragName,
	                 MainAddrType*    mainAddr )
	{
		typedef CFragLoadOptions_Traits< options > Traits;
		
		CFragConnectionID connID;
		SymbolAddressPtr tempMainAddr;
		
		GetDiskFragment( file,
		                 offset,
		                 length,
		                 fragName,
		                 CFragLoadOptions( options ),
		                 &connID,
		                 &tempMainAddr );
		
		if ( mainAddr != NULL )
		{
			*mainAddr = SymAddr_Cast< MainAddrType >( tempMainAddr );
		}
		
		return Traits::MakeResult( connID );
	}
	
	template < CFragLoadOptions options >
	inline typename CFragLoadOptions_Traits< options >::Result
	//
	GetDiskFragment( const FSSpec&    file,
	                 std::size_t      offset   = 0,
	                 std::size_t      length   = kCFragGoesToEOF,
	                 ConstStr63Param  fragName = NULL )
	{
		return GetDiskFragment< options, SymbolAddressPtr >( file,
		                                                     offset,
		                                                     length,
		                                                     fragName,
		                                                     NULL );
	}
	
	
	// GetMemFragment
	
	void GetMemFragment( const void*         memAddr,
	                     std::size_t         length,
	                     ConstStr63Param     fragName,
	                     CFragLoadOptions    options,
	                     CFragConnectionID*  connID   = NULL,
	                     SymbolAddressPtr*   mainAddr = NULL );
	
	template < CFragLoadOptions options, class MainAddrType >
	inline typename CFragLoadOptions_Traits< options >::Result
	//
	GetMemFragment( const void*      memAddr,
	                std::size_t      length,
	                ConstStr63Param  fragName,
	                MainAddrType*    mainAddr )
	{
		typedef CFragLoadOptions_Traits< options > Traits;
		
		CFragConnectionID connID;
		SymbolAddressPtr tempMainAddr;
		
		GetMemFragment( memAddr,
		                length,
		                fragName,
		                CFragLoadOptions( options ),
		                &connID,
		                &tempMainAddr );
		
		if ( mainAddr != NULL )
		{
			*mainAddr = SymAddr_Cast< MainAddrType >( tempMainAddr );
		}
		
		return Traits::MakeResult( connID );
	}
	
	template < CFragLoadOptions options >
	inline typename CFragLoadOptions_Traits< options >::Result
	//
	GetMemFragment( const void*      memAddr,
	                std::size_t      length,
	                ConstStr63Param  fragName = NULL )
	{
		return GetMemFragment< options, SymbolAddressPtr >( memAddr,
		                                                    length,
		                                                    fragName,
		                                                    NULL );
	}
	
	void CloseConnection( nucleus::owned< CFragConnectionID > connID );
	
	// 384
	void FindSymbol( CFragConnectionID  connID,
	                 ConstStr255Param   symName,
	                 SymbolAddressPtr*  symAddr,
	                 CFragSymbolClass*  symClass );
	
	template < class SymAddrType >
	inline void FindSymbol( CFragConnectionID  connID,
	                        ConstStr255Param   symName,
	                        SymAddrType*       symAddr,
	                        CFragSymbolClass*  symClass = NULL )
	{
		SymbolAddressPtr tempSymAddr;
		
		FindSymbol( connID, symName, &tempSymAddr, symClass );
		
		if ( symAddr != NULL )
		{
			*symAddr = SymAddr_Cast< SymAddrType >( tempSymAddr );
		}
	}
	
	template < class SymAddrType >
	inline SymAddrType FindSymbol( CFragConnectionID  connID, 
	                               ConstStr255Param   symName, 
	                               CFragSymbolClass*  symClass = NULL )
	{
		SymAddrType result;
		
		FindSymbol< SymAddrType >( connID, symName, &result, symClass );
		
		return result;
	}
	
	std::size_t CountSymbols( CFragConnectionID connID );
	
	void GetIndSymbol( CFragConnectionID  connID,
	                   std::size_t        symIndex,
	                   Str255             symName,
	                   SymbolAddressPtr*  symAddr,
	                   CFragSymbolClass*  symClass );
	
	template < class SymAddrType >
	inline void GetIndSymbol( CFragConnectionID  connID,
	                          std::size_t        symIndex,
	                          Str255             symName,
	                          SymAddrType*       symAddr,
	                          CFragSymbolClass*  symClass = NULL )
	{
		SymbolAddressPtr tempSymAddr;
		
		GetIndSymbol( connID, symIndex, symName, &tempSymAddr, symClass );
		
		if ( symAddr != NULL )
		{
			*symAddr = SymAddr_Cast< SymAddrType >( tempSymAddr );
		}
	}
	
	// ...
	
#endif  // #if ! __LP64__
	
}

#endif
