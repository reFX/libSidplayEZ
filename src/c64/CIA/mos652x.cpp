/*
* This file is part of libsidplayfp, a SID player engine.
*
* Copyright 2011-2022 Leandro Nini <drfiemost@users.sourceforge.net>
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

#include <cstring>

#include "mos652x.h"
#include "sidendian.h"

namespace libsidplayfp
{

enum
{
	DDRA = 2,
	DDRB = 3,
	TAL = 4,
	TAH = 5,
	TBL = 6,
	TBH = 7,
	TOD_TEN = 8,
	TOD_SEC = 9,
	TOD_MIN = 10,
	TOD_HR = 11,
	SDR = 12,
	ICR = 13,
	IDR = 13,
	CRA = 14,
	CRB = 15
};
//-----------------------------------------------------------------------------

// Timer A
void TimerA::underFlow ()
{
	parent.underflowA ();
}
//-----------------------------------------------------------------------------

// Timer B
void TimerB::underFlow ()
{
	parent.underflowB ();
}
//-----------------------------------------------------------------------------

// Interrupt Source 8521
void InterruptSource8521::trigger ( uint8_t interruptMask )
{
	if ( InterruptSource::isTriggered ( interruptMask ) )
		schedule ( 0 );
}
//-----------------------------------------------------------------------------

// Interrupt Source 6526
void InterruptSource6526::trigger ( uint8_t interruptMask )
{
	// interrupts are delayed by 1 clk on old CIAs
	if ( InterruptSource::isTriggered ( interruptMask ) )
		schedule ( 1 );

	// if timer B underflows during the acknowledge cycle
	// it triggers an interrupt as expected
	// but the second bit in icr is not set
	if ( interruptMask == INTERRUPT_UNDERFLOW_B && ack0 () )
	{
		idr &= ~INTERRUPT_UNDERFLOW_B;
		idrTemp &= ~INTERRUPT_UNDERFLOW_B;
	}
}
//-----------------------------------------------------------------------------

uint8_t InterruptSource6526::clear ()
{
	auto	oldIdr = InterruptSource::clear ();
	idr &= INTERRUPT_REQUEST;

	return oldIdr;
}
//-----------------------------------------------------------------------------

const char* MOS652X::credits ()
{
	return	"MOS6526/8521 (CIA) Emulation:\n"
			"\tCopyright (C) 2001-2004 Simon White\n"
			"\tCopyright (C) 2007-2010 Antti S. Lankila\n"
			"\tCopyright (C) 2009-2014 VICE Project\n"
			"\tCopyright (C) 2011-2021 Leandro Nini\n";
}
//-----------------------------------------------------------------------------

MOS652X::MOS652X ( EventScheduler& scheduler )
	: interruptSource6526 ( scheduler, *this )
	, interruptSource8521 ( scheduler, *this )
	, eventScheduler ( scheduler )
	, timerA ( scheduler, *this )
	, timerB ( scheduler, *this )
	, interruptSource ( &interruptSource6526 )
	, tod ( scheduler, *this, regs )
	, bTickEvent ( "CIA B counts A", *this, &MOS652X::bTick )
{
	MOS652X::reset ();
}
//-----------------------------------------------------------------------------

void MOS652X::reset ()
{
	std::fill_n ( regs, std::size ( regs ), 0 );

	// Reset timers
	timerA.reset ();
	timerB.reset ();

	// Reset interruptSource
	interruptSource->reset ();

	// Reset tod
	tod.reset ();

	eventScheduler.cancel ( bTickEvent );
}
//-----------------------------------------------------------------------------

uint8_t MOS652X::adjustDataPort ( uint8_t data )
{
	if ( regs[ CRA ] & 0x02 )
	{
		data &= 0xbf;
		if ( timerA.getPb ( regs[ CRA ] ) )
			data |= 0x40;
	}

	if ( regs[ CRB ] & 0x02 )
	{
		data &= 0x7f;
		if ( timerB.getPb ( regs[ CRB ] ) )
			data |= 0x80;
	}
	return data;
}
//-----------------------------------------------------------------------------

uint8_t MOS652X::read ( uint8_t addr )
{
	addr &= 0x0f;

	timerA.syncWithCpu ();
	timerA.wakeUpAfterSyncWithCpu ();
	timerB.syncWithCpu ();
	timerB.wakeUpAfterSyncWithCpu ();

	switch ( addr )
	{
		case TAL:		return endian_get16_lo8 ( timerA.getTimer () );
		case TAH:		return endian_get16_hi8 ( timerA.getTimer () );
		case TBL:		return endian_get16_lo8 ( timerB.getTimer () );
		case TBH:		return endian_get16_hi8 ( timerB.getTimer () );

		case TOD_TEN:
		case TOD_SEC:
		case TOD_MIN:
		case TOD_HR:
			return tod.read ( addr - TOD_TEN );

		case IDR:		return interruptSource->clear ();
		case CRA:		return ( regs[ CRA ] & 0xee ) | ( timerA.getState () & 1 );
		case CRB:		return ( regs[ CRB ] & 0xee ) | ( timerB.getState () & 1 );

		default:		return regs[ addr ];
	}
}
//-----------------------------------------------------------------------------

void MOS652X::write ( uint8_t addr, uint8_t data )
{
	addr &= 0x0f;

	timerA.syncWithCpu ();
	timerB.syncWithCpu ();

	const auto	oldData = regs[ addr ];
	regs[ addr ] = data;

	switch ( addr )
	{
		case TAL:		timerA.latchLo ( data );			break;
		case TAH:		timerA.latchHi ( data );			break;
		case TBL:		timerB.latchLo ( data );			break;
		case TBH:		timerB.latchHi ( data );			break;

		case TOD_TEN:
		case TOD_SEC:
		case TOD_MIN:
		case TOD_HR:
			tod.write ( addr - TOD_TEN, data );
			break;

		case SDR:
			break;

		case ICR:
			interruptSource->set ( data );
			break;

		case CRA:
			// Reset the underflow flipflop for the data port
			if ( ( data & 1 ) && ! ( oldData & 1 ) )
				timerA.setPbToggle ( true );

			timerA.setControlRegister ( data );
			break;

		case CRB:
			// Reset the underflow flipflop for the data port
			if ( ( data & 1 ) && ! ( oldData & 1 ) )
				timerB.setPbToggle ( true );

			timerB.setControlRegister ( data | ( data & 0x40 ) >> 1 );
			break;
	}

	timerA.wakeUpAfterSyncWithCpu ();
	timerB.wakeUpAfterSyncWithCpu ();
}
//-----------------------------------------------------------------------------

void MOS652X::bTick ()
{
	timerB.cascade ();
}
//-----------------------------------------------------------------------------

void MOS652X::underflowA ()
{
	interruptSource->trigger ( InterruptSource::INTERRUPT_UNDERFLOW_A );

	if ( ( regs[ CRB ] & 0x41 ) == 0x41 )
		if ( timerB.started () )
			eventScheduler.schedule ( bTickEvent, 0, EVENT_CLOCK_PHI2 );
}
//-----------------------------------------------------------------------------

void MOS652X::underflowB ()
{
	interruptSource->trigger ( InterruptSource::INTERRUPT_UNDERFLOW_B );
}
//-----------------------------------------------------------------------------

void MOS652X::todInterrupt ()
{
	interruptSource->trigger ( InterruptSource::INTERRUPT_ALARM );
}
//-----------------------------------------------------------------------------

void MOS652X::spInterrupt ()
{
	interruptSource->trigger ( InterruptSource::INTERRUPT_SP );
}
//-----------------------------------------------------------------------------

void MOS652X::setModel ( model_t model )
{
	if ( model == model_t::MOS6526 )
		interruptSource = &interruptSource6526;
	else
		interruptSource = &interruptSource8521;
}
//-----------------------------------------------------------------------------
}
