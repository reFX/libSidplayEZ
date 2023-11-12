/*
* This file is part of libsidplayfp, a SID player engine.
*
* Copyright 2011-2015 Leandro Nini <drfiemost@users.sourceforge.net>
* Copyright 2007-2010 Antti Lankila
* Copyright 2004,2010 Dag Lem <resid@nimrod.no>
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

#include "Filter6581.h"

namespace reSIDfp
{
//-----------------------------------------------------------------------------

Filter6581::Filter6581 ()
	: f0_dac ( FilterModelConfig6581::getInstance ()->getDAC ( 0.5 ) )
	, hpIntegrator ( FilterModelConfig6581::getInstance ()->buildIntegrator () )
	, bpIntegrator ( FilterModelConfig6581::getInstance ()->buildIntegrator () )
{
	mixer = FilterModelConfig6581::getInstance ()->getMixer ();
	summer = FilterModelConfig6581::getInstance ()->getSummer ();
	gain_res = FilterModelConfig6581::getInstance ()->getGainRes ();
	gain_vol = FilterModelConfig6581::getInstance ()->getGainVol ();
	voiceScaleS11 = FilterModelConfig6581::getInstance ()->getVoiceScaleS11 ();
	voiceDC = FilterModelConfig6581::getInstance ()->getNormalizedVoiceDC ();

	ve = mixer[ 0 ][ 0 ];
}
//-----------------------------------------------------------------------------

void Filter6581::updatedCenterFrequency ()
{
	const auto	Vw = f0_dac[ fc ];

	hpIntegrator->setVw ( Vw );
	bpIntegrator->setVw ( Vw );
}
//-----------------------------------------------------------------------------

void Filter6581::setFilterCurve ( double curvePosition )
{
	delete[] f0_dac;
	f0_dac = FilterModelConfig6581::getInstance ()->getDAC ( curvePosition );
	updatedCenterFrequency ();
}
//-----------------------------------------------------------------------------

} // namespace reSIDfp
