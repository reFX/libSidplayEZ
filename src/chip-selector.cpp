#include "chip-selector.h"

#include <algorithm>

namespace libsidplayfp
{

//-----------------------------------------------------------------------------

std::pair<std::string, ChipSelector::settings> ChipSelector::getChipProfile ( const char* _path, const char* _filename )
{
	auto	path = std::string ( _path );

	// Normalize path separators
	std::replace ( path.begin (), path.end (), '\\', '/' );

	// Remove root
	auto	pos = path.rfind ( "/MUSICIANS/" );

	// If tune is not from a "/MUSICIANS/" folder, return default values
	if ( pos == std::string::npos )
		return {};

	path = path.substr ( pos );

	std::string	bestPath;
	std::string	bestProfile;

	// Identify author by folder (longest matching path wins)
	for ( const auto& [ name, set ] : chipProfiles )
	{
		if ( ! path.starts_with ( set.folder ) )
			continue;

		if ( set.folder.size () < bestPath.size () )
			continue;

		bestPath = set.folder;
		bestProfile = name;
	}

	// No profile found, return defaults
	if ( bestProfile.empty () )
		return {};

	// Get author profile
	const auto&	set = chipProfiles.at ( bestProfile );

	// No exceptions, return profile
	if ( set.exceptions.empty () )
		return std::make_pair ( bestProfile, set );

	// Get filename without extension
	auto	filename = std::string ( _filename );
	filename.erase ( filename.length () - 4 );

	// Find new author if exception matches
	if ( auto exception = set.exceptions.find ( filename ); exception != set.exceptions.end () )
		return std::make_pair ( exception->second, chipProfiles.at ( exception->second ) );

	// No exception matched, return best profile
	return std::make_pair ( bestProfile, set );
}
//-----------------------------------------------------------------------------

void ChipSelector::setProfiles ( const profileMap& map )
{
	chipProfiles = map;
}
//-----------------------------------------------------------------------------

}
