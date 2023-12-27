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
		double		zeroDac = 0.4;
		std::unordered_map<std::string, std::string>	exceptions;
	};

	using profileMap = std::unordered_map<std::string, settings>;

	std::pair<std::string, settings> getChipProfile ( const char* path, const char* filename );
	void setProfiles ( const profileMap& map );

private:
	profileMap	chipProfiles = {
		#include "chip-profiles.h"
	};
};
//-----------------------------------------------------------------------------

}
