#pragma once
/*
* This file is part of libsidplayfp, a SID player engine.
*
* Copyright 2011-2018 Leandro Nini <drfiemost@users.sourceforge.net>
* Copyright 2009-2014 VICE Project
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

#include <stdint.h>
#include <array>

#include "EventScheduler.h"

namespace libsidplayfp
{

class MOS652X;

/**
* TOD implementation taken from Vice.
*/
class Tod : private Event
{
private:
	enum
	{
		TENTHS = 0,
		SECONDS = 1,
		MINUTES = 2,
		HOURS = 3
	};

private:
	// Event scheduler.
	EventScheduler&	eventScheduler;

	// Reference to the MOS6526 which this Timer belongs to
	MOS652X&	parent;

	const uint8_t&	cra;
	const uint8_t&	crb;

	event_clock_t	cycles;
	event_clock_t	period = ~0;	// Dummy

	unsigned int	todtickcounter = 0;

	bool	isLatched;
	bool	isStopped;

	std::array<uint8_t, 4>	clock;
	std::array<uint8_t, 4>	latch;
	std::array<uint8_t, 4>	alarm;

private:
	inline void checkAlarm ();
	inline void updateCounters ();

	void event () override;

public:
	Tod ( EventScheduler& scheduler, MOS652X& _parent, uint8_t regs[ 0x10 ] )
		: Event ( "CIA Time of Day" )
		, eventScheduler ( scheduler )
		, parent ( _parent )
		, cra ( regs[ 0x0e ] )
		, crb ( regs[ 0x0f ] )
	{
	}

	/**
	* Reset TOD.
	*/
	void reset ();

	/**
	* Read TOD register.
	*
	* @param addr
	*            register register to read
	*/
	uint8_t read ( uint8_t reg );

	/**
	* Write TOD register.
	*
	* @param addr
	*            register to write
	* @param data
	*            value to write
	*/
	void write ( uint8_t reg, uint8_t data );

	/**
	* Set TOD period.
	*
	* @param clock
	*/
	void setPeriod ( event_clock_t _clock ) { period = _clock * ( 1 << 7 ); }
};

}
