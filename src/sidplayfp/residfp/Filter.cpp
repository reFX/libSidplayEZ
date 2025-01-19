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

#include "Filter.h"
#include "FilterModelConfig.h"

#include <array>

namespace reSIDfp
{

//-----------------------------------------------------------------------------

Filter::Filter ( FilterModelConfig& _fmc )
	: fmc ( _fmc )
{
	// Pre-calculate all possible summer/mixer combinations
	for ( auto i = 0u; i < std::size ( sumFltResults ); ++i )
	{
		auto	Nsum = 0;
		auto	Nmix = 0;

		if ( i & 1 )	{ Nsum += 0x10; } else { Nmix++; }
	 	if ( i & 2 )	{ Nsum += 0x10; } else { Nmix++; }
	 	if ( i & 4 )	{ Nsum += 0x10; } else if ( ! ( i & 0x80 ) )	{ Nmix++; }
	 	if ( i & 8 )	{ Nsum += 0x10; } else { Nmix++; }

		if ( i & 0x10 )	Nmix++;
		if ( i & 0x20 )	Nmix++;
		if ( i & 0x40 )	Nmix++;

		sumFltResults[ i ] = uint8_t ( Nsum | Nmix );
	}

	mixer = fmc.getMixer ();
	summer = fmc.getSummer ();
	volume = fmc.getVolume ();
	resonance = fmc.getResonance ();
}
//-----------------------------------------------------------------------------

void Filter::reset ()
{
	writeFC_LO ( 0 );
	writeFC_HI ( 0 );
	writeMODE_VOL ( 0 );
	writeRES_FILT ( 0 );
}
//-----------------------------------------------------------------------------

void Filter::writeFC_LO ( uint8_t fc_lo )
{
	fc = ( fc & 0x7F8 ) | ( fc_lo & 7 );

	updatedCenterFrequency ();
}
//-----------------------------------------------------------------------------

void Filter::writeFC_HI ( uint8_t fc_hi )
{
	fc = ( ( fc_hi << 3 ) & 0x7F8 ) | ( fc & 7 );

	updatedCenterFrequency ();
}
//-----------------------------------------------------------------------------

void Filter::writeRES_FILT ( uint8_t res_filt )
{
	filterModeRouting = ( filterModeRouting & 0xF0 ) | ( res_filt & 0x0F );

	currentResonance = resonance + ( ( res_filt >> 4 ) << 16 );

	updateMixing ();
}
//-----------------------------------------------------------------------------

void Filter::writeMODE_VOL ( uint8_t mode_vol )
{
	filterModeRouting = ( filterModeRouting & 0x0F ) | ( mode_vol & 0xF0 );

	currentVolume = volume + ( ( mode_vol & 0x0F ) << 16 );

	updateMixing ();
}
//-----------------------------------------------------------------------------

} // namespace reSIDfp
