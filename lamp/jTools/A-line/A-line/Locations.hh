/*	============
 *	Locations.hh
 *	============
 */

#ifndef A_LINE_LOCATIONS_HH
#define A_LINE_LOCATIONS_HH

// Standard C++
#include <string>


namespace ALine
{
	
	std::string UserSrcTreePath();
	
	std::string ProjectSourcesPath ( const std::string& projectPath );
	std::string ProjectIncludesPath( const std::string& projectPath );
	
	std::string SourceDotListFile( const std::string& projectPath );
	
	std::string TargetDirPath( const std::string& target );
	
	std::string LibrariesDirPath();
	std::string RezzedDirPath();
	
	std::string ProjectDiagnosticsDirPath( const std::string& proj );
	std::string ProjectPrecompiledDirPath( const std::string& proj );
	std::string ProjectObjectsDirPath    ( const std::string& proj );
	std::string ProjectMetadataDirPath   ( const std::string& proj );
	std::string ProjectLinkedDirPath     ( const std::string& proj );
	std::string ProjectOutputDirPath     ( const std::string& proj );
	
}

#endif

