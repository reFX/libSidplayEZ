/*
* This file is part of libsidplayfp, a SID player engine.
*
* Copyright 2011-2024 Leandro Nini <drfiemost@users.sourceforge.net>
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

#include "Filter8580.h"
#include "Integrator8580.h"

namespace reSIDfp
{

/**
* W/L ratio of frequency DAC bit 0, other bits are proportional.
* When no bits are selected a resistance with half W/L ratio is selected.
*/
constexpr auto	DAC_WL0 = 0.00615;

//-----------------------------------------------------------------------------

Filter8580::Filter8580 ()
	: Filter ( FilterModelConfig8580::getInstance ()->getVoiceScaleS11 () )
	, voiceDC ( FilterModelConfig8580::getInstance ()->getNormalizedVoiceDC ( 4.76 ) )
	, hpIntegrator ( FilterModelConfig8580::getInstance ()->buildIntegrator () )
	, bpIntegrator ( FilterModelConfig8580::getInstance ()->buildIntegrator () )
{
	mixer = FilterModelConfig8580::getInstance ()->getMixer ();
	summer = FilterModelConfig8580::getInstance ()->getSummer ();
	gain_res = FilterModelConfig8580::getInstance ()->getGainRes ();
	gain_vol = FilterModelConfig8580::getInstance ()->getGainVol ();

	ve = mixer[ 0 ][ 0 ];

	setFilterCurve ( 0.5 );
}
//-----------------------------------------------------------------------------

void Filter8580::updatedCenterFrequency ()
{
	auto	wl = 0.0;
	auto	dacWL = DAC_WL0;

	if ( fc )
	{
		for ( auto i = 0u; i < 11; i++ )
		{
			if ( fc & ( 1 << i ) )
				wl += dacWL;

			dacWL *= 2.0;
		}
	}
	else
	{
		wl = dacWL / 2.0;
	}

	hpIntegrator->setFc ( wl );
	bpIntegrator->setFc ( wl );
}
//-----------------------------------------------------------------------------

void Filter8580::setFilterCurve ( double curvePosition )
{
	// Adjust cp
	// 1.2 <= cp <= 1.8
	cp = 1.2 + curvePosition * 3.0 / 5.0;

	hpIntegrator->setV ( cp );
	bpIntegrator->setV ( cp );
}
//-----------------------------------------------------------------------------

} // namespace reSIDfp
