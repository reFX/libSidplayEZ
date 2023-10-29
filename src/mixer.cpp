/*
* This file is part of libsidplayfp, a SID player engine.
*
* Copyright 2011-2023 Leandro Nini <drfiemost@users.sourceforge.net>
* Copyright 2007-2010 Antti Lankila
* Copyright 2000 Simon White
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

#include <cassert>
#include <algorithm>

#include "mixer.h"
#include "sidemu.h"

namespace libsidplayfp {

//-----------------------------------------------------------------------------

void Mixer::clockChips ()
{
	for ( auto chp : m_chips )
		chp->clock ();
}
//-----------------------------------------------------------------------------

void Mixer::resetBufs ()
{
	for ( auto chp : m_chips )
		chp->bufferpos ( 0 );
}
//-----------------------------------------------------------------------------

void Mixer::doMix ()
{
	auto	buf = m_sampleBuffer + m_sampleIndex;

	// extract buffer info now that the SID is updated
	// clock() may update bufferpos
	// NB: if more than one chip exists, their bufferpos is identical to first chip's
	const auto	sampleCount = m_chips.front ()->bufferpos ();

	auto	i = 0;

	// Specialization for one chip, mono out
	if ( m_buffers.size () == 1 && m_stereo == false )
	{
		const auto	toCopy = std::min ( sampleCount, int ( m_sampleCount - m_sampleIndex ) );

		std::copy_n ( m_buffers[ 0 ], toCopy, buf );

		m_sampleIndex += toCopy;
		i = toCopy;
	}
	else
	{
		const auto	channels = m_stereo ? 2u : 1u;

		while ( i < sampleCount )
		{
			// Handle whatever output the sid has generated so far
			if ( m_sampleIndex >= m_sampleCount )
				break;

 			// Are there enough samples to generate the next one?
			if ( ( i + 1 ) >= sampleCount )
				break;

			m_iSamples[ 0 ] = m_buffers[ 0 ][ i ];

			for ( auto k = 1; k < int ( m_buffers.size () ); k++ )
				m_iSamples[ k ] = m_buffers[ k ][ i ];

			// increment i to mark we ate some samples
			i++;

			*buf++ = int16_t ( ( this->*( m_mix[ 0 ] ) ) () );
			if ( channels == 2 )
				*buf++ = int16_t ( ( this->*( m_mix[ 1 ] ) ) () );

			m_sampleIndex += channels;
		}
	}

	// move the unhandled data to start of buffer, if any
	const auto	samplesLeft = sampleCount - i;

	for ( auto bfr : m_buffers )
		std::copy_n ( bfr + i, samplesLeft, bfr );

	for ( auto chp : m_chips )
		chp->bufferpos ( samplesLeft );
}
//-----------------------------------------------------------------------------

void Mixer::init ( int16_t* buffer, uint32_t count )
{
	// don't allow odd counts for stereo playback
	assert ( m_stereo == false || ( count & 1 ) == 0 );

	// we need a minimum buffer-size, otherwise a crash might occur
	assert ( count > ( ( m_stereo + 1 ) * 100 ) );

	m_sampleIndex = 0;
	m_sampleCount = count;
	m_sampleBuffer = buffer;
}
//-----------------------------------------------------------------------------

void Mixer::updateParams ()
{
	switch ( m_buffers.size () )
	{
		case 1:
			m_mix[ 0 ] = m_stereo ? &Mixer::stereo_OneChip : &Mixer::mono1;
			m_mix[ 1 ] = &Mixer::stereo_OneChip;
			break;

		case 2:
			m_mix[ 0 ] = m_stereo ? &Mixer::stereo_ch1_TwoChips : &Mixer::mono2;
			m_mix[ 1 ] = &Mixer::stereo_ch2_TwoChips;
			break;

		case 3:
			m_mix[ 0 ] = m_stereo ? &Mixer::stereo_ch1_ThreeChips : &Mixer::mono3;
			m_mix[ 1 ] = &Mixer::stereo_ch2_ThreeChips;
			break;
	}
}
//-----------------------------------------------------------------------------

void Mixer::clearSids ()
{
	m_chips.clear ();
	m_buffers.clear ();
}
//-----------------------------------------------------------------------------

void Mixer::addSid ( sidemu* chip )
{
	if ( ! chip )
		return;

	m_chips.push_back ( chip );
	m_buffers.push_back ( chip->buffer () );

	updateParams ();
}
//-----------------------------------------------------------------------------

void Mixer::setStereo ( bool stereo )
{
	if ( m_stereo == stereo )
		return;

	m_stereo = stereo;

	updateParams ();
}
//-----------------------------------------------------------------------------

void Mixer::setSamplerate ( uint32_t rate )
{
	m_sampleRate = rate;
}
//-----------------------------------------------------------------------------

}
