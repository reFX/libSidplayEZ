#pragma once
/*
* This file is part of libsidplayfp, a SID player engine.
*
* Copyright 2011-2016 Leandro Nini <drfiemost@users.sourceforge.net>
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

#include <memory>
#include <algorithm>

namespace reSIDfp
{
	typedef enum { MOS6581 = 1, MOS8580 } ChipModel;
}

#include "Filter6581.h"
#include "Filter8580.h"
#include "ExternalFilter.h"
#include "Voice.h"
#include "resample/TwoPassSincResampler.h"

namespace reSIDfp
{

//-----------------------------------------------------------------------------

/**
* SID error exception.
*/
class SIDError
{
private:
	const char* message;

public:
	SIDError ( const char* msg )
		: message ( msg )
	{
	}

	const char* getMessage () const { return message; }
};
//-----------------------------------------------------------------------------

/**
* MOS6581/MOS8580 emulation.
*/
class SID final
{
private:
	// Currently active filter
	Filter*	filter;

	// Filter used, if model is set to 6581
	Filter6581	filter6581;

	// Filter used, if model is set to 8580
	Filter8580	filter8580;

	// External filter that provides high-pass and low-pass filtering to adjust sound tone slightly
	ExternalFilter	externalFilter;

	// Table of waveforms
	std::vector<int16_t>	waveTable;
	std::vector<int16_t>	pulldownTable;

	// Resampler used by audio generation code
	TwoPassSincResampler	resampler;

	static constexpr int	numVoices = 3;

	// SID voices
	Voice	voice[ numVoices ];

	// Time to live for the last written value
	int busValueTtl;

	// Current chip model's bus value TTL
	int modelTTL;

	// Time until #voiceSync must be run.
	unsigned int nextVoiceSync;

	// Currently active chip model.
	ChipModel model;

	// Last written value
	unsigned char busValue;

	/**
	* Emulated nonlinearity of the envelope DAC.
	*
	* @See Dac
	*/
	float	envDAC[ 256 ];

	/**
	* Emulated nonlinearity of the oscillator DAC.
	*
	* @See Dac
	*/
	float	oscDAC[ 4096 ];

private:
	/**
	* Get output sample.
	*
	* @return the output sample
	*/
	inline int output ()
	{
		const auto  v1 = voice[ 0 ].output ( voice[ 2 ].waveformGenerator );
		const auto  v2 = voice[ 1 ].output ( voice[ 0 ].waveformGenerator );
		const auto  v3 = voice[ 2 ].output ( voice[ 1 ].waveformGenerator );

		return externalFilter.clock ( filter->clock ( v1, v2, v3 ) );
	}

	/**
	* Calculate the number of cycles according to current parameters
	* that it takes to reach sync
	*
	* @param sync whether to do the actual voice synchronization
	*/
	inline void voiceSync ( bool sync )
	{
		// Synchronize the 3 waveform generators
		if ( sync )
		{
			voice[ 0 ].waveformGenerator.synchronize ( voice[ 1 ].waveformGenerator, voice[ 2 ].waveformGenerator );
			voice[ 1 ].waveformGenerator.synchronize ( voice[ 2 ].waveformGenerator, voice[ 0 ].waveformGenerator );
			voice[ 2 ].waveformGenerator.synchronize ( voice[ 0 ].waveformGenerator, voice[ 1 ].waveformGenerator );
		}

		// Calculate the time to next voice sync
		nextVoiceSync = std::numeric_limits<int>::max ();

		for ( auto i = 0; i < numVoices; i++ )
		{
			auto&		wave = voice[ i ].waveformGenerator;
			const auto	freq = wave.readFreq ();

			if ( wave.readTest () || freq == 0 || ! voice[ ( i + 1 ) % 3 ].waveformGenerator.readSync () )
				continue;

			const auto	accumulator = wave.readAccumulator ();
			const auto	thisVoiceSync = ( ( 0x7fffff - accumulator ) & 0xffffff ) / freq + 1;

			if ( thisVoiceSync < nextVoiceSync )
				nextVoiceSync = thisVoiceSync;
		}
	}

public:
	SID ();

	/**
	* Set chip model.
	*
	* @param model chip model to use
	* @throw SIDError
	*/
	void setChipModel ( ChipModel model );

	/**
	* Get currently emulated chip model.
	*/
	ChipModel getChipModel () const { return model; }

	/**
	* SID reset.
	*/
	void reset ();

	/**
	* Read registers
	*
	* Reading a write only register returns the last char written to any SID register.
	* The individual bits in this value start to fade down towards zero after a few cycles.
	* All bits reach zero within approximately $2000 - $4000 cycles.
	* It has been claimed that this fading happens in an orderly fashion,
	* however sampling of write only registers reveals that this is not the case.
	* NOTE: This is not correctly modeled.
	* The actual use of write only registers has largely been made
	* in the belief that all SID registers are readable.
	* To support this belief the read would have to be done immediately
	* after a write to the same register (remember that an intermediate write
	* to another register would yield that value instead).
	* With this in mind we return the last value written to any SID register
	* for $2000 cycles without modeling the bit fading.
	*
	* @param offset SID register to read
	* @return value read from chip
	*/
	unsigned char read ( int offset );

	/**
	* Write registers.
	*
	* @param offset chip register to write
	* @param value value to write
	*/
	void write ( int offset, unsigned char value );

	/**
	* Setting of SID sampling parameters.
	*
	* Use a clock frequency of 985248Hz for PAL C64, 1022730Hz for NTSC C64.
	* The default end of passband frequency is pass_freq = 0.9*sample_freq/2
	* for sample frequencies up to ~ 44.1kHz, and 20kHz for higher sample frequencies.
	*
	* For resampling, the ratio between the clock frequency and the sample frequency
	* is limited as follows: 125*clock_freq/sample_freq < 16384
	* E.g. provided a clock frequency of ~ 1MHz, the sample frequency can not be set
	* lower than ~ 8kHz. A lower sample frequency would make the resampling code
	* overfill its 16k sample ring buffer.
	*
	* The end of passband frequency is also limited: pass_freq <= 0.9*sample_freq/2
	*
	* E.g. for a 44.1kHz sampling rate the end of passband frequency
	* is limited to slightly below 20kHz.
	* This constraint ensures that the FIR table is not overfilled.
	*
	* @param clockFrequency System clock frequency at Hz
	* @param samplingFrequency Desired output sampling rate
	* @param highestAccurateFrequency
	* @throw SIDError
	*/
	void setSamplingParameters ( double clockFrequency, double samplingFrequency, double highestAccurateFrequency );

	/**
	* Clock SID forward using chosen output sampling algorithm.
	*
	* @param cycles c64 clocks to clock
	* @param buf audio output buffer
	* @return number of samples produced
	*/
	inline int clock ( unsigned int cycles, int16_t* buf )
	{
		if ( busValueTtl )
		{
			if ( busValueTtl -= cycles; busValueTtl <= 0 )
			{
				busValue = 0;
				busValueTtl = 0;
			}
		}

		auto    s = 0;

		while ( cycles )
		{
			if ( auto delta_t = std::min ( nextVoiceSync, cycles ); delta_t > 0 )
			{
				for ( auto i = 0u; i < delta_t; i++ )
				{
					// clock waveform generators
					voice[ 0 ].waveformGenerator.clock ();
					voice[ 1 ].waveformGenerator.clock ();
					voice[ 2 ].waveformGenerator.clock ();

					// clock envelope generators
					voice[ 0 ].envelopeGenerator.clock ();
					voice[ 1 ].envelopeGenerator.clock ();
					voice[ 2 ].envelopeGenerator.clock ();

					if ( resampler.input ( output () ) )
						buf[ s++ ] = int16_t ( resampler.output () );
				}

				cycles -= delta_t;
				nextVoiceSync -= delta_t;
			}

			if ( ! nextVoiceSync )
				voiceSync ( true );
		}

		return s;
	}

	/**
	* Set filter curve parameter for 6581 model.
	*
	* @see Filter6581::setFilterCurve(double)
	*/
	void setFilter6581Curve ( double filterCurve )	{	filter6581.setFilterCurve ( filterCurve );	}

	/**
	* Set filter curve parameter for 8580 model.
	*
	* @see Filter8580::setFilterCurve(double)
	*/
	void setFilter8580Curve ( double filterCurve )	{	filter8580.setFilterCurve ( filterCurve );	}

	float getEnvLevel ( int voiceNo ) const	{	return voice[voiceNo].getEnvLevel();	}
};
//-----------------------------------------------------------------------------

} // namespace reSIDfp
