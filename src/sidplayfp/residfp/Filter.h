#pragma once
/*
* This file is part of libsidplayfp, a SID player engine.
*
* Copyright 2011-2024 Leandro Nini <drfiemost@users.sourceforge.net>
* Copyright 2007-2010 Antti Lankila
* Copyright 2004 Dag Lem <resid@nimrod.no>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <stdint.h>

#include "FilterModelConfig.h"

namespace reSIDfp
{
/**
* SID filter base class
*/
class Filter
{
protected:
	FilterModelConfig&	fmc;

	uint16_t**	mixer = nullptr;
	uint16_t**	summer = nullptr;
	uint16_t*	resonance = nullptr;
	uint16_t*	volume = nullptr;

	// Current volume amplifier setting.
	uint16_t*	currentVolume = nullptr;

	// Current filter/voice mixer setting.
	uint16_t*	currentMixer = nullptr;

	// Filter input summer setting.
	uint16_t*	currentSummer = nullptr;

	// Filter resonance value.
	uint16_t*	currentResonance = nullptr;

	// Filter highpass state
	int Vhp = 0;

	// Filter bandpass state
	int Vbp = 0;

	// Filter lowpass state
	int Vlp = 0;

	// Filter external input
	int Ve = 0;

	// Filter cutoff frequency
	unsigned int fc = 0;

	// Switch voice 3 off
	int		voice3Mask = UINT_MAX;

	uint8_t	filterModeRouting = 0;		// bits = mute vce3, hp, bp, lp, fltE, flt3, flt2, flt1
	uint8_t	sumFltResults[ 256 ];		// Precalculate all possible summers and filter mixers

	/**
	* Set filter cutoff frequency.
	*/
	virtual void updatedCenterFrequency () = 0;

	/**
	* Mixing configuration modified (offsets change)
	*/
	inline void updateMixing ()
	{
		// Voice 3 is silenced by voice3off if it is not routed through the filter
		voice3Mask = ( filterModeRouting & 0x84 ) == 0x80 ? 0 : -1;

		const auto	Nsum_Nmix = sumFltResults[ filterModeRouting ];

		currentSummer = summer[ Nsum_Nmix >> 4 ];
		currentMixer = mixer[ Nsum_Nmix & 0xF ];
	}

public:
	Filter ( FilterModelConfig& fmc );
	virtual ~Filter () = default;

	/**
	* SID clocking - 1 cycle
	*
	* @param v1 voice 1 in
	* @param v2 voice 2 in
	* @param v3 voice 3 in
	* @return filtered output
	*/
	virtual inline uint16_t clock ( float voice1, float voice2, float voice3 )
	{
		const auto	Vsum	= fmc.getNormalizedVoice ( voice1 )
							+ fmc.getNormalizedVoice ( voice2 )
							+ ( fmc.getNormalizedVoice ( voice3 ) & voice3Mask )
							+ Ve;

		return currentVolume[ currentMixer[ Vsum ] ];
	}

	/**
	* SID reset.
	*/
	void reset ();

	/**
	* Write Frequency Cutoff Low register.
	*
	* @param fc_lo Frequency Cutoff Low-Byte
	*/
	void writeFC_LO ( uint8_t fc_lo );

	/**
	* Write Frequency Cutoff High register.
	*
	* @param fc_hi Frequency Cutoff High-Byte
	*/
	void writeFC_HI ( uint8_t fc_hi );

	/**
	* Write Resonance/Filter register.
	*
	* @param res_filt Resonance/Filter
	*/
	void writeRES_FILT ( uint8_t res_filt );

	/**
	* Write filter Mode/Volume register.
	*
	* @param mode_vol Filter Mode/Volume
	*/
	void writeMODE_VOL ( uint8_t mode_vol );
};

} // namespace reSIDfp
