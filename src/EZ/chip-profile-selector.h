#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

namespace libsidplayEZ
{
//-----------------------------------------------------------------------------

class ChipProfileSelector final
{
public:
	enum : uint8_t
	{
		weak,
		average,
		strong,
	};

	struct settings final
	{
		std::string	name;
		std::string	folder;

		// Status (approved or best-guess)
		bool		isApproved = false;

		// Filter settings
		bool		fltCapOld = false;
		double		flt0Dac = 0.4;
		double		fltGain = 0.92;
		double		fltSaturation = 1.0;
		double		fltResonance = 1.0;		// resonance strength, 1 = stock chip

		// 6581 waveform DAC DC offset, envelope-scaled (1 = real chip's full
		// offset); with extInDC and voiceBias this sets the digi loudness
		double		waveDC = 0.5;

		// EXT-IN DC scale (1 = chip default, 0 = grounded pin)
		double		extInDC = 1.0;

		// Voice DC bias, per-chip spread of the ~5V operating point:
		// -1 .. 1, 0 = nominal chip, ~ -5 .. +5 dB of digi loudness
		double		voiceBias = 0.0;

		// 6581 charge-leakage rate (1.0 = R4-class/warm chip, ~10 = R3-class/warm, lower = colder)
		double		leakageRate = 1.0;

		// 6581 combined waveform strength (the 8580 keeps the emulation default)
		int			cwsLevel = average;
		bool		cwsSawPulseUltra = false;

		// Exceptions (selects another authors chip, usually for collaborations);
		// the map expands the csv cell's subtune ranges, the string is the cell verbatim
		std::unordered_map<std::string, std::string>	exceptions;
		std::string										exceptionsCsv;
	};

	using profileMap = std::unordered_map<std::string, settings>;

	settings getProfile ( const char* path, const char* filename, const int subtune ) const;

	// Returns a description of the first unusable cell, empty when the file was clean.
	std::string setProfiles ( const std::string& csvStr, bool merge = false );

private:
	profileMap	chipProfiles;
};
//-----------------------------------------------------------------------------

}
