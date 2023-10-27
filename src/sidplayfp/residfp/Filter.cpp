/*
* This file is part of libsidplayfp, a SID player engine.
*
* Copyright 2011-2013 Leandro Nini <drfiemost@users.sourceforge.net>
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

namespace reSIDfp
{

//-----------------------------------------------------------------------------

void Filter::reset ()
{
	writeFC_LO ( 0 );
	writeFC_HI ( 0 );
	writeMODE_VOL ( 0 );
	writeRES_FILT ( 0 );
}
//-----------------------------------------------------------------------------

void Filter::writeFC_LO ( unsigned char fc_lo )
{
	fc = ( fc & 0x7f8 ) | ( fc_lo & 0x007 );
	updatedCenterFrequency ();
}
//-----------------------------------------------------------------------------

void Filter::writeFC_HI ( unsigned char fc_hi )
{
	fc = ( fc_hi << 3 & 0x7f8 ) | ( fc & 0x007 );

	updatedCenterFrequency ();
}
//-----------------------------------------------------------------------------

void Filter::writeRES_FILT ( unsigned char res_filt )
{
	filt = res_filt;

	updateResonance ( ( res_filt >> 4 ) & 0x0f );

	filt1 = filt & 0x01;
	filt2 = filt & 0x02;
	filt3 = filt & 0x04;
	filtE = filt & 0x08;

	updatedMixing ();
}
//-----------------------------------------------------------------------------

void Filter::writeMODE_VOL ( unsigned char mode_vol )
{
	vol = mode_vol & 0x0f;

	lp = mode_vol & 0x10;
	bp = mode_vol & 0x20;
	hp = mode_vol & 0x40;

	voice3off = mode_vol & 0x80;

	updatedMixing ();
}
//-----------------------------------------------------------------------------

void Filter::updatedMixing ()
{
	currentGain = gain_vol[ vol ];

	auto	ni = 0u;
	auto	no = 0u;

	if ( filt1 ) { ni++; } else { no++; }
	if ( filt2 ) { ni++; } else { no++; }
	if ( filt3 ) { ni++; } else if ( ! voice3off ) { no++; }

	no++;
//	if ( filtE ) { ni++; } else { no++; }

	currentSummer = summer[ ni ];

	if ( lp ) no++;
	if ( bp ) no++;
	if ( hp ) no++;

	currentMixer = mixer[ no ];
}
//-----------------------------------------------------------------------------

} // namespace reSIDfp
