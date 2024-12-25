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

//-----------------------------------------------------------------------------

sidemu::sidemu ( EventScheduler& _eventScheduler )
	: eventScheduler ( _eventScheduler )
{
	reset ( 0xF );
}
//-----------------------------------------------------------------------------

void sidemu::reset ( uint8_t volume )
{
	std::fill ( std::begin ( lastpoke ), std::end ( lastpoke ), 0 );

	m_accessClk = 0;
	m_sid.reset ();
	m_sid.write ( 0x18, volume );
}
//-----------------------------------------------------------------------------

}
