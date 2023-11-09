/*
* This file is part of libsidplayfp, a SID player engine.
*
* Copyright 2011-2015 Leandro Nini <drfiemost@users.sourceforge.net>
* Copyright 2007-2010 Antti Lankila
* Copyright 2000-2001 Simon White
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

#include "sidemu.h"

namespace libsidplayfp
{

const char sidemu::ERR_UNSUPPORTED_FREQ[] = "Unable to set desired output frequency.";
const char sidemu::ERR_INVALID_SAMPLING[] = "Invalid sampling method.";
const char sidemu::ERR_INVALID_CHIP[] = "Invalid chip model.";

//-----------------------------------------------------------------------------

sidemu::sidemu ()
{
	reset ( 0 );
}
//-----------------------------------------------------------------------------

void sidemu::reset ( uint8_t volume )
{
	c64sid::reset ();

	m_accessClk = 0;
	m_sid.reset ();
	m_sid.write ( 0x18, volume );
}
//-----------------------------------------------------------------------------

bool sidemu::lock ( EventScheduler* scheduler )
{
	if ( isLocked )
		return false;

	isLocked = true;
	eventScheduler = scheduler;

	return true;
}
//-----------------------------------------------------------------------------

void sidemu::unlock ()
{
	isLocked = false;
	eventScheduler = nullptr;
}
//-----------------------------------------------------------------------------

void sidemu::model ( SidConfig::sid_model_t _model )
{
	m_sid.setChipModel ( _model == SidConfig::MOS6581 ? reSIDfp::MOS6581 : reSIDfp::MOS8580 );
	m_status = true;
}
//-----------------------------------------------------------------------------

void sidemu::sampling ( float systemfreq, float outputfreq )
{
	try
	{
		const auto  halfFreq = int ( std::min ( outputfreq * ( 20000.0f / 44100.0f ), 20000.0f ) );
		m_sid.setSamplingParameters ( systemfreq, outputfreq, halfFreq );
	}
	catch ( reSIDfp::SIDError const& )
	{
		m_status = false;
		m_error = ERR_UNSUPPORTED_FREQ;
		return;
	}

	m_status = true;
}
//-----------------------------------------------------------------------------

}
