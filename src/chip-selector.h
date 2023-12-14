#pragma once

#include <unordered_map>
#include <string>

namespace libsidplayfp
{
//-----------------------------------------------------------------------------

class ChipSelector final
{
public:
	struct settings 
	{
		std::string	folder;
		double		filter = 0.5;
		std::unordered_map<std::string, std::string>	exceptions;
	};

	settings getChipProfile ( const char* path, const char* filename );

	std::unordered_map<std::string, settings>	chipProfiles = {
		#include "chip-profiles.h"
	};
};
//-----------------------------------------------------------------------------

}
