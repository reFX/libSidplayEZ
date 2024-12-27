#pragma once
/*
* This file is part of libsidplayfp, a SID player engine.
*
* Copyright 2011-2023 Leandro Nini <drfiemost@users.sourceforge.net>
* Copyright 2007-2010 Antti Lankila
* Copyright 2004, 2010 Dag Lem <resid@nimrod.no>
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

#include "FilterModelConfig8580.h"

#include <stdint.h>
#include <cassert>

namespace reSIDfp
{

/**
* 8580 integrator
*
*                   +---C---+
*                   |       |
*     vi -----Rfc---o--[A>--o-- vo
*                   vx
*
*     IRfc + ICr = 0
*     IRfc + C*(vc - vc0)/dt = 0
*     dt/C*(IRfc) + vc - vc0 = 0
*     vc = vc0 - n*(IRfc(vi,vx))
*     vc = vc0 - n*(IRfc(vi,g(vc)))
*
* IRfc = K*W/L*(Vgst^2 - Vgdt^2) = n*((Vddt - vx)^2 - (Vddt - vi)^2)
*
* Rfc gate voltage is generated by an OP Amp and depends on chip temperature.
*/
class Integrator8580 final
{
private:
	int	vx = 0;
	int	vc = 0;

	uint16_t	nVgt;
	uint16_t	n_dac;

	const FilterModelConfig8580& fmc;

public:
	Integrator8580 ( const FilterModelConfig8580* _fmc )
		: fmc ( *_fmc )
	{
		setV ( 1.5 );
	}

	/**
	* Set Filter Cutoff resistor ratio.
	*/
	sidinline void setFc ( double wl )
	{
		// Normalized current factor, 1 cycle at 1MHz.
		// Fit in 5 bits.
		n_dac = fmc.getNormalizedCurrentFactor ( wl );
	}

	/**
	* Set FC gate voltage multiplier.
	*/
	void setV ( double v )
	{
		// Gate voltage is controlled by the switched capacitor voltage divider
		// Ua = Ue * v = 4.75v  1<v<2
		assert ( v > 1.0 && v < 2.0 );
		const auto	Vg = fmc.getVref () * v;
		const auto	Vgt = Vg - fmc.getVth ();

		// Vg - Vth, normalized so that translated values can be subtracted:
		// Vgt - x = (Vgt - t) - (x - t)
		nVgt = fmc.getNormalizedValue ( Vgt );
	}

	sidinline int solve ( int vi )
	{
		// Make sure we're not in subthreshold mode
		assert ( vx < nVgt );

		// DAC voltages
		const unsigned int Vgst = nVgt - vx;
		const unsigned int Vgdt = ( vi < nVgt ) ? nVgt - vi : 0;  // triode/saturation mode

		const unsigned int Vgst_2 = Vgst * Vgst;
		const unsigned int Vgdt_2 = Vgdt * Vgdt;

		// DAC current, scaled by (1/m)*2^13*m*2^16*m*2^16*2^-15 = m*2^30
		const auto	n_I_dac = n_dac * ( int ( Vgst_2 - Vgdt_2 ) >> 15 );

		// Change in capacitor charge.
		vc += n_I_dac;

		// vx = g(vc)
		const auto	tmp = ( vc >> 15 ) + ( 1 << 15 );
		assert ( tmp < ( 1 << 16 ) );
		vx = fmc.getOpampRev ( tmp );

		// Return vo
		return vx - ( vc >> 14 );
	}
};

} // namespace reSIDfp
