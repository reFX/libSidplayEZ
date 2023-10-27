/*
* This file is part of libsidplayfp, a SID player engine.
*
* Copyright 2011-2023 Leandro Nini <drfiemost@users.sourceforge.net>
* Copyright 2007-2010 Antti Lankila
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

#include "WaveformCalculator.h"

#include <cmath>
#include <map>
#include <mutex>

namespace reSIDfp
{
typedef std::map<const CombinedWaveformConfig*, matrix_t> cw_cache_t;

cw_cache_t PULLDOWN_CACHE;
std::mutex PULLDOWN_CACHE_Lock;

WaveformCalculator* WaveformCalculator::getInstance ()
{
	static WaveformCalculator instance;
	return &instance;
}
//-----------------------------------------------------------------------------

/**
* Parameters derived with the Monte Carlo method based on
* samplings by kevtris. Code and data available in the project repository [1].
*
* The score here reported is the acoustic error
* calculated XORing the estimated and the sampled values.
* In parentheses the number of mispredicted bits.
*
* [1] https://github.com/libsidplayfp/combined-waveforms
*/
const CombinedWaveformConfig config[ 2 ][ 5 ] =
{
	{ /* kevtris chip G (6581 R2) */
		{0.862147212f, 0.f,          10.8962431f,    2.50848103f }, // TS  error  1941 (327/28672)
		{0.932746708f, 2.07508397f,   1.03668225f,   1.14876997f }, // PT  error  5992 (126/32768)
		{0.860927045f, 2.43506575f,   0.908603609f,  1.07907593f }, // PS  error  3693 (521/28672)
		{0.741343081f, 0.0452554375f, 1.1439606f,    1.05711341f }, // PTS error   338 ( 29/28672)
		{0.96f,        2.5f,          1.1f,          1.2f        }, // NP  guessed
	},
	{ /* kevtris chip V (8580 R5) */
		{0.715788841f, 0.f,           1.32999945f,   2.2172699f  }, // TS  error   928 (135/32768)
		{0.93500334f,  1.05977178f,   1.08629429f,   1.43518543f }, // PT  error  7991 (212/32768)
		{0.920648575f, 0.943601072f,  1.13034654f,   1.41881108f }, // PS  error 12566 (394/32768)
		{0.90921098f,  0.979807794f,  0.942194462f,  1.40958893f }, // PTS error  2092 ( 60/32768)
		{0.95f,        1.15f,         1.f,           1.45f       }, // NP  guessed
	},
};
//-----------------------------------------------------------------------------

/**
* Generate bitstate based on emulation of combined waves pulldown.
*
* @param distancetable
* @param pulsestrength
* @param threshold
* @param accumulator the high bits of the accumulator value
*/
short calculatePulldown ( float distancetable[], float pulsestrength, float threshold, unsigned int accumulator )
{
	unsigned char   bit[ 12 ];

	for ( auto i = 0; i < 12; i++ )
		bit[ i ] = ( accumulator & ( 1 << i ) ) ? 1 : 0;

	float   pulldown[ 12 ];

	for ( auto sb = 0; sb < 12; sb++ )
	{
		auto    avg = 0.0f;
		auto    n = 0.0f;

		for ( auto cb = 0; cb < 12; cb++ )
		{
			if ( cb == sb )
				continue;

			const auto  weight = distancetable[ sb - cb + 12 ];
			avg += ( 1 - bit[ cb ] ) * weight;
			n += weight;
		}

		avg -= pulsestrength;

		pulldown[ sb ] = avg / n;
	}

	// Get the predicted value
	short   value = 0;

	for ( auto i = 0; i < 12; i++ )
	{
		const auto  bitValue = bit[ i ] != 0 ? 1.0f - pulldown[ i ] : 0.0f;

		if ( bitValue > threshold )
			value |= 1 << i;
	}

	return value;
}
//-----------------------------------------------------------------------------

WaveformCalculator::WaveformCalculator ()
	: wftable ( 4, 4096 )
{
	/// Calculate triangle waveform
	auto triXor = [] ( unsigned int val )
	{
		return ( ( ( val & 0x800 ) == 0 ) ? val : ( val ^ 0xfff ) ) << 1;
	};

	// Build waveform table
	for ( unsigned int idx = 0; idx < ( 1 << 12 ); idx++ )
	{
		const auto  saw = uint16_t ( idx );
		const auto  tri = uint16_t ( triXor ( idx ) );

		wftable[ 0 ][ idx ] = 0x0FFF;
		wftable[ 1 ][ idx ] = tri;
		wftable[ 2 ][ idx ] = saw;
		wftable[ 3 ][ idx ] = saw & ( saw << 1 );
	}
}
//-----------------------------------------------------------------------------

matrix_t* WaveformCalculator::buildPulldownTable ( ChipModel model )
{
	std::lock_guard<std::mutex> lock ( PULLDOWN_CACHE_Lock );

	const auto  cfgArray = config[ model == MOS6581 ? 0 : 1 ];

	auto    lb = PULLDOWN_CACHE.lower_bound ( cfgArray );

	if ( lb != PULLDOWN_CACHE.end () && ! ( PULLDOWN_CACHE.key_comp ()( cfgArray, lb->first ) ) )
		return &( lb->second );

	matrix_t	pdTable ( 5, 4096 );
	for ( auto wav = 0; wav < 5; wav++ )
	{
		const auto&	cfg = cfgArray[ wav ];

		float distancetable[ 12 * 2 + 1 ];
		distancetable[ 12 ] = 1.0f;
		for ( auto i = 12; i > 0; i-- )
		{
			distancetable[ 12 - i ] = std::powf ( cfg.distance1, -i );
			distancetable[ 12 + i ] = std::powf ( cfg.distance2, -i );
		}

		for ( auto idx = 0u; idx < ( 1 << 12 ); idx++ )
			pdTable[ wav ][ idx ] = calculatePulldown ( distancetable, cfg.pulsestrength, cfg.threshold, idx );
	}

	return &( PULLDOWN_CACHE.emplace_hint ( lb, cw_cache_t::value_type ( cfgArray, pdTable ) )->second );
}
//-----------------------------------------------------------------------------

} // namespace reSIDfp
