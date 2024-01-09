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

namespace reSIDfp
{

/**
* SID filter base class
*/
class Filter
{
protected:
	uint16_t**	mixer = nullptr;
	uint16_t**	summer = nullptr;
	uint16_t**	gain_res = nullptr;
	uint16_t**	gain_vol = nullptr;

	const int voiceScaleS11;

	// Current volume amplifier setting.
	uint16_t*	currentGain = nullptr;

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
	int ve = 0;

	// Filter cutoff frequency
	unsigned int fc = 0;

	// Routing to filter or outside filter
	bool	filt1 = false;
	bool	filt2 = false;
	bool	filt3 = false;
	bool	filtE = false;

	// Switch voice 3 off
	bool	voice3off = false;

	// Highpass, bandpass, and lowpass filter modes
	bool	hp = false;
	bool	bp = false;
	bool	lp = false;

	// Current volume.
	uint8_t	vol = 0;

	uint8_t	filtResMode = 0;		// Selects which inputs to route through filter
	uint8_t	sumFltResults[ 256 ];	// Precalculate all possible summers and filter mixers

	/**
	* Set filter cutoff frequency.
	*/
	virtual void updatedCenterFrequency () = 0;

	/**
	* Set filter resonance.
	*/
	virtual void updateResonance ( uint8_t res ) = 0;

	/**
	* Mixing configuration modified (offsets change)
	*/
	inline void updateMixing ()
	{
		currentGain = gain_vol[ vol ];

		const auto	ni_no = sumFltResults[ filtResMode ];

		currentSummer = summer[ ni_no >> 4 ];
		currentMixer = mixer[ ni_no & 0xF ];
	}

	virtual int getVoiceDC ( int env ) const = 0;

public:
	Filter ( int _voiceScaleS11 );
	virtual ~Filter () = default;

	/**
	* SID clocking - 1 cycle
	*
	* @param v1 voice 1 in
	* @param v2 voice 2 in
	* @param v3 voice 3 in
	* @return filtered output
	*/
	virtual uint16_t clock ( int v1, int v2, int v3 ) = 0;

	inline int getNormalizedVoice ( int val, int env ) const	{	return ( val * voiceScaleS11 / ( 1 << 15 ) ) + getVoiceDC ( env );	}

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
