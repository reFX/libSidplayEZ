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
#include <cassert>

namespace reSIDfp
{

//-----------------------------------------------------------------------------

/**
* Combined waveform model parameters
*/
using distance_t = float ( * )( float, int );

using CombinedWaveformConfig = struct
{
	distance_t distFunc;
	float threshold;
	float topbit;
	float pulsestrength;
	float distance1;
	float distance2;
};

// Distance functions
static float exponentialDistance ( float distance, int i )
{
	return std::pow ( distance, float ( -i ) );
}

static float linearDistance ( float distance, int i )
{
	return 1.0f / ( 1.0f + i * distance );
}

static float quadraticDistance ( float distance, int i )
{
	return 1.0f / ( 1.0f + ( i * i ) * distance );
}

/**
 * Parameters derived with the Monte Carlo method based on
 * samplings from real machines.
 * Code and data available in the project repository [1].
 * Sampling program made by Dag Lem [2].
 *
 * The score here reported is the acoustic error
 * calculated XORing the estimated and the sampled values.
 * In parentheses the number of mispredicted bits.
 *
 * [1] https://github.com/libsidplayfp/combined-waveforms
 * [2] https://github.com/daglem/reDIP-SID/blob/master/research/combsample.d64
 */
const CombinedWaveformConfig	config[ 3 ][ 2 ][ 5 ] =
{
	// Weak
	{
		{	// 6581 R2 4383 sampled by ltx128
			// TS  error 1858 (204/32768)
			{ exponentialDistance, 0.886832297f, 1.0f, 0.0f, 2.14438701f, 9.51839447f },
			// PT  error  612 (102/32768)
			{ linearDistance, 1.01262534f, 1.0f, 2.46070528f, 0.0537485816f, 0.0986242667f },
			// PS  error 8135 (575/32768)
			{ linearDistance, 2.14896345f, 1.0216713f, 10.5400085f, 0.244498149f, 0.126134038f },
			// PTS error 2505 (63/32768)
			{ linearDistance, 1.29061747f, 0.9754318f, 3.15377498f, 0.0968349651f, 0.318573922f },
			// NP  guessed
			{ exponentialDistance, 0.96f, 1.0f, 2.5f, 1.1f, 1.2f },
		},
		{	// 8580 R5 1087 sampled by reFX-Mike
			// TS  error 1627 (137/32768)
			{ exponentialDistance, 0.795011938f, 1.54905677f, 0.0f, 1.79432333f, 2.24898171f },
			// PT  error 7898 (162/32768)
			{ exponentialDistance, 0.9482705f, 1.0f, 1.21793139f, 1.04166055f, 1.37272894f },
			// PS  error 9804 (337/32768)
			{ quadraticDistance, 0.954935849f, 1.00321376f, 1.28759611f, 0.000331178948f, 0.151375741f },
			// PTS error 3184 (56/32768)
			{ linearDistance, 0.945096612f, 1.06510091f, 0.905796111f, 1.05054963f, 1.4661454f },
			// NP  guessed
			{ exponentialDistance, 0.95f, 1.0f, 1.15f, 1.0f, 1.45f },
		},
	},
	// Average
	{
		{	// 6581 R3 4785 sampled by Trurl
			// TS  error 2298 (339/32768)
			{ exponentialDistance, 0.776678205f, 1.18439901f, 0.0f, 2.25732255f, 5.12803745f },
			// PT  error  582 (57/32768)
			{ linearDistance, 1.01866758f, 1.0f, 2.69177628f, 0.0233543925f, 0.0850229636f },
			// PS  error 9242 (679/32768)
			{ linearDistance, 2.20329857f, 1.04501438f, 10.5146885f, 0.277294368f, 0.143747061f },
			// PTS error 2799 (71/32768)
			{ linearDistance, 1.35652959f, 1.09051275f, 3.21098137f, 0.16658926f, 0.370252877f },
			// NP  guessed
			{ exponentialDistance, 0.96f, 1.0f, 2.5f, 1.1f, 1.2f },
		},
		{	// 8580 R5 5092 25 sampled by reFX-Mike
			// TS  error 1212 (183/32768)
			{ exponentialDistance, 0.684999049f, 0.916620493f, 0.0f, 1.14715648f, 2.02339816f },
			// PT  error 6153 (295/32768)
			{ exponentialDistance,  0.940367579f, 1.0f, 1.26695442f, 0.976729453f, 1.57954705f },
			// PS  error 7620 (454/32768)
			{ quadraticDistance, 0.963866293f, 1.22095084f, 1.01380754f, 0.0110885892f, 0.381492466f },
			// PTS error 3701 (117/32768)
			{ linearDistance, 0.976761818f, 0.202727556f, 0.988633931f, 0.939373314f, 9.37139416f },
			// NP  guessed
			{ exponentialDistance, 0.95f, 1.0f, 1.15f, 1.0f, 1.45f },
		},
	},
	// Strong
	{
		{	// 6581 R2 0384 sampled by Trurl
			// TS  error 20337 (1579/32768)
			{ exponentialDistance, 0.000637792516f, 1.56725872f, 0.0f, 0.00036806846f, 1.51800942f },
			// PT  error  5194 (240/32768)
			{ linearDistance, 0.924824238f, 1.0f, 1.96749473f, 0.0891806409f, 0.234794483f },
			// PS  error 31015 (2181/32768)
			{ linearDistance, 1.2328074f, 0.73079139f, 3.9719491f, 0.00156516861f, 0.314677745f },
			// PTS error  9874 (201/32768)
			{ linearDistance, 1.08558261f, 0.857638359f, 1.52781796f, 0.152927235f, 1.02657032f },
			// NP  guessed
			{ exponentialDistance, 0.96f, 1.0f, 2.5f, 1.1f, 1.2f },
		},
		{	// 8580 R5 1489 sampled by reFX-Mike
			// TS  error 4837 (388/32768)
			{ exponentialDistance, 0.89762634f, 56.7594185f, 0.0f, 7.68995237f, 12.0754194f },
			// PT  error 9298 (506/32768)
			{ exponentialDistance,  0.867885351f, 1.0f, 1.4511894f, 1.07057536f, 1.43333757f },
			// PS  error 13168 (718/32768)
			{ quadraticDistance, 0.89255774f, 1.2253896f, 1.75615835f, 0.0245045591f, 0.12982437f },
			// PTS error 6879 (309/32768)
			{ linearDistance, 0.913530529f, 0.96415776f, 0.931084037f, 1.05731869f, 1.80506349f },
			// NP  guessed
			{ exponentialDistance, 0.95f, 1.0f, 1.15f, 1.0f, 1.45f },
		},
	},
};
//-----------------------------------------------------------------------------

std::vector<int16_t> WaveformCalculator::buildWaveTable ()
{
	std::vector<int16_t>	waveTable ( 4 * 4096 );

	// Calculate triangle waveform
	auto triXor = [] ( int val )
	{
		return ( ( ( val & 0x800 ) == 0 ) ? val : ( val ^ 0xfff ) ) << 1;
	};

	// Build waveform table
	for ( auto idx = 0; idx < 4096; idx++ )
	{
		const auto  saw = int16_t ( idx );
		const auto  tri = int16_t ( triXor ( idx ) );

		waveTable[ ( 0 << 12 ) + idx ] = 0x0FFF;
		waveTable[ ( 1 << 12 ) + idx ] = tri;
		waveTable[ ( 2 << 12 ) + idx ] = saw;
		waveTable[ ( 3 << 12 ) + idx ] = int16_t ( saw & ( saw << 1 ) );
	}

	return waveTable;
}
//-----------------------------------------------------------------------------

/**
* Generate bitstate based on emulation of combined waves pulldown
*
* @param distancetable
* @param pulsestrength
* @param threshold
* @param accumulator the high bits of the accumulator value
*/
static int16_t calculatePulldown ( float distancetable[], float topbit, float pulsestrength, float threshold, unsigned int accumulator )
{
	float	bit[ 12 ];

	for ( auto i = 0u; i < 12; i++ )
		bit[ i ] = ( accumulator & ( 1 << i ) ) ? 1.0f : 0.0f;

	bit[ 11 ] = uint8_t ( bit[ 11 ] * topbit );

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
			avg += ( 1.0f - bit[ cb ] ) * weight;
			n += weight;
		}

		avg -= pulsestrength;

		pulldown[ sb ] = avg / n;
	}

	// Get the predicted value
	int16_t	value = 0;

	for ( auto i = 0; i < 12; i++ )
	{
		const auto	bitValue = bit[ i ] > 0.0f ? 1.0f - pulldown[ i ] : 0.0f;

		if ( bitValue > threshold )
			value |= 1u << i;
	}

	return value;
}
//-----------------------------------------------------------------------------

void WaveformCalculator::buildPulldownTable ( std::vector<int16_t>& pulldownTable, const bool is6581, const int combinedWaveformStrength, const float threshold )
{
	pulldownTable.resize ( 5 * 4096 );

	const auto	modelIdx = is6581 ? 0 : 1;

	for ( auto wav = 0; wav < 5; wav++ )
	{
		const auto&	cfg = config[ combinedWaveformStrength ][ modelIdx ][ wav ];

		float	distancetable[ 12 * 2 + 1 ];
		distancetable[ 12 ] = 1.0f;
		for ( auto i = 12; i > 0; i-- )
		{
			distancetable[ 12 - i ] = cfg.distFunc ( cfg.distance1, i );
			distancetable[ 12 + i ] = cfg.distFunc ( cfg.distance2, i );
		}

		for ( auto idx = 0u; idx < 4096; idx++ )
			pulldownTable[ wav * 4096 + idx ] = calculatePulldown ( distancetable, cfg.topbit, cfg.pulsestrength, cfg.threshold * threshold, idx );
	}
}
//-----------------------------------------------------------------------------

} // namespace reSIDfp
