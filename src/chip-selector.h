#pragma once

#include <unordered_map>
#include <string>

namespace libsidplayfp
{
//-----------------------------------------------------------------------------

class ChipSelector final
{
public:
	enum
	{
		weak,
		average,
		strong,
	};

	struct settings final
	{
		std::string	folder;
		double		filter = 0.5;
		double		digi = 1.0;
		double		zeroDac = 0.4;
		int			cwsLevel = strong;
		double		cwsThreshold = 0.8;
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
