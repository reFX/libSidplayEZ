#pragma once
/*
* This file is part of libsidplayfp, a SID player engine.
*
* Copyright 2023 Michael Hartmann
* Copyright 2011-2015 Leandro Nini <drfiemost@users.sourceforge.net>
* Copyright 2007-2010 Antti Lankila
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

#include <cmath>
#include <memory>

#include "SincResampler.h"

namespace reSIDfp
{
/**
* Compose a more efficient SINC from chaining two other SINCs
*/
class TwoPassSincResampler final
{
public:
	void setup ( double clockFrequency, double samplingFrequency, double highestAccurateFrequency )
	{
		// Calculation according to Laurent Ganier. It evaluates to about 120 kHz at typical settings.
		// Some testing around the chosen value seems to confirm that this does work.
		const auto	intermediateFrequency =
				2.0 * highestAccurateFrequency
			+	std::sqrt	( 2.0 * highestAccurateFrequency * clockFrequency
								* ( samplingFrequency - 2.0 * highestAccurateFrequency ) / samplingFrequency
							);

		s1.setup ( clockFrequency, intermediateFrequency, highestAccurateFrequency );
		s2.setup ( intermediateFrequency, samplingFrequency, highestAccurateFrequency );
	}

	inline bool input ( int sample )
	{
		if ( s1.input ( sample ) )
			return s2.input ( s1.output () );

		return false;
	}

	inline int output () const
	{
		return s2.output ();
	}

	void reset ()
	{
		s1.reset ();
		s2.reset ();
	}

private:
	SincResampler	s1;
	SincResampler	s2;
};

} // namespace reSIDfp
