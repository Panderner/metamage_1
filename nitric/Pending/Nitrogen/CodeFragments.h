// CodeFragments.h

#ifndef NITROGEN_CODEFRAGMENTS_H
#define NITROGEN_CODEFRAGMENTS_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __CODEFRAGMENTS__
#include FRAMEWORK_HEADER(CarbonCore,CodeFragments.h)
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_CFBUNDLE_H
#include "Nitrogen/CFBundle.h"
#endif
#ifndef NITROGEN_FILES_H
#include "Nitrogen/Files.h"
#endif
#ifndef NITROGEN_MULTIPROCESSING_H
#include "Nitrogen/Multiprocessing.h"
#endif
#ifndef NITROGEN_RESOURCES_H
#include "Nitrogen/Resources.h"
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
	
	void RegisterCodeFragmentManagerErrors();
	
	#pragma mark -
	#pragma mark � General Types and Constants �
	
	class CFragArchitecture_Tag {};
	typedef SelectorType< CFragArchitecture_Tag, ::CFragArchitecture, kAnyCFragArch > CFragArchitecture;
	
	inline ResType CFragResourceType()     { return ResType::Make( kCFragResourceType    ); }
	inline ResID   CFragResourceID()       { return ResID  ::Make( kCFragResourceID      ); }
	inline OSType  CFragLibraryFileType()  { return OSType ::Make( kCFragLibraryFileType ); }
	// kCFragAllFileTypes
	
	inline CFragArchitecture PowerPCCFragArch()      { return CFragArchitecture::Make( kPowerPCCFragArch     ); }
	inline CFragArchitecture Motorola68KCFragArch()  { return CFragArchitecture::Make( kMotorola68KCFragArch ); }
	inline CFragArchitecture AnyCFragArch()          { return CFragArchitecture::Make( kAnyCFragArch         ); }
	
	inline CFragArchitecture CompiledCFragArch()     { return CFragArchitecture::Make( kCompiledCFragArch    ); }
	
	inline std::size_t CFragGoesToEOF()  { return kCFragGoesToEOF; }
	
	using ::CFragConnectionID;
	
	template <>
	struct Disposer< CFragConnectionID > : public std::unary_function< CFragConnectionID, void >,
	                                       private DefaultDestructionOSStatusPolicy
	{
		void operator()( CFragConnectionID connID ) const
		{
			OnlyOnce< RegisterCodeFragmentManagerErrors >();
			HandleDestructionOSStatus( ::CloseConnection( &connID ) );
		}
	};
	
	class CFragLoadOptions_Tag {};
	typedef FlagType< CFragLoadOptions_Tag, ::CFragLoadOptions, 0 > CFragLoadOptions;
	
	class CFragSymbolClass_Tag {};
	typedef SelectorType< CFragSymbolClass_Tag, ::CFragSymbolClass, 0 > CFragSymbolClass;
	
	inline CFragLoadOptions ReferenceCFrag()    { return CFragLoadOptions::Make( kReferenceCFrag   ); }
	inline CFragLoadOptions FindCFrag()         { return CFragLoadOptions::Make( kFindCFrag        ); }
	inline CFragLoadOptions PrivateCFragCopy()  { return CFragLoadOptions::Make( kPrivateCFragCopy ); }
	
	inline CFragSymbolClass CodeCFragSymbol()     { return CFragSymbolClass::Make( kCodeCFragSymbol    ); }
	inline CFragSymbolClass DataCFragSymbol()     { return CFragSymbolClass::Make( kDataCFragSymbol    ); }
	inline CFragSymbolClass TVectorCFragSymbol()  { return CFragSymbolClass::Make( kTVectorCFragSymbol ); }
	inline CFragSymbolClass TOCCFragSymbol()      { return CFragSymbolClass::Make( kTOCCFragSymbol     ); }
	inline CFragSymbolClass GlueCFragSymbol()     { return CFragSymbolClass::Make( kGlueCFragSymbol    ); }
	
	// Opaque pointer type
	typedef struct SymbolAddress* SymbolAddressPtr;
	
	// 333
	void GetDiskFragment( const FSSpec& file, 
	                      std::size_t offset, 
	                      std::size_t length, 
	                      ConstStr63Param fragName, 
	                      CFragLoadOptions findFlags, 
	                      CFragConnectionID* connID = NULL, 
	                      SymbolAddressPtr* mainAddr = NULL, 
	                      ::Str255 errName = NULL );
	
	template < ::CFragLoadOptions findFlags >  struct GetDiskFragment_Traits;
	
	struct Shared_Connection_Traits
	{
		typedef CFragConnectionID Result;
		
		static Result MakeResult( CFragConnectionID connID )  { return connID; }
	};
	
	template <>  struct GetDiskFragment_Traits< kReferenceCFrag > : Shared_Connection_Traits {};
	template <>  struct GetDiskFragment_Traits< kFindCFrag      > : Shared_Connection_Traits {};
	
	template <>  struct GetDiskFragment_Traits< kPrivateCFragCopy >
	{
		typedef Owned< CFragConnectionID > Result;
		
		static Result MakeResult( CFragConnectionID connID )
		{
			return Owned< CFragConnectionID >::Seize( connID );
		}
	};
	
	template < class SymbolAddressType >
	SymbolAddressType SymAddr_Cast( SymbolAddressPtr symAddr )
	{
		// The double reinterpret_cast is necessary if SymbolAddressType is a 
		// pointer-to-function.  Gcc doesn't like casting a pointer-to-object
		// to a pointer-to-function, so we use long as an intermediary.
		
		return reinterpret_cast< SymbolAddressType >( reinterpret_cast< long >( symAddr ) );
	}
	
	template < ::CFragLoadOptions findFlags, class MainAddrType >
	typename GetDiskFragment_Traits< findFlags >::Result GetDiskFragment
	(
		const FSSpec&    file, 
		std::size_t      offset, 
		std::size_t      length, 
		ConstStr63Param  fragName, 
	    MainAddrType*    mainAddr, 
	    ::Str255         errName  = NULL )
	{
		typedef GetDiskFragment_Traits< findFlags > Traits;
		
		CFragConnectionID connID;
		SymbolAddressPtr tempMainAddr;
		
		GetDiskFragment
		(
			file, offset, length, fragName, CFragLoadOptions( findFlags ), 
			&connID, &tempMainAddr, errName
		);
		
		if ( mainAddr != NULL )
		{
			*mainAddr = SymAddr_Cast< MainAddrType >( tempMainAddr );
		}
		
		return Traits::MakeResult( connID );
	}
	
	template < ::CFragLoadOptions findFlags >
	typename GetDiskFragment_Traits< findFlags >::Result GetDiskFragment
	(
		const FSSpec&    file, 
		std::size_t      offset   = 0, 
		std::size_t      length   = kCFragGoesToEOF, 
		ConstStr63Param  fragName = NULL, 
	    ::Str255         errName  = NULL )
	{
		return GetDiskFragment< findFlags, SymbolAddressPtr >
		(
			file, offset, length, fragName, NULL, errName
		);
	}
	
	// 384
	void FindSymbol( CFragConnectionID  connID, 
	                 ConstStr255Param   symName, 
	                 SymbolAddressPtr*  symAddr, 
	                 CFragSymbolClass*  symClass );
	
	template < class SymAddrType >
	void FindSymbol( CFragConnectionID  connID, 
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
	
}

#endif
