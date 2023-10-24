/*
 * This file is part of libsidplayfp, a SID player engine.
 *
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

#include "OpAmp.h"

namespace reSIDfp
{

constexpr auto	EPSILON = 1e-8;

double OpAmp::solve ( double n, double vi )
{
    // Start off with an estimate of x and a root bracket [ak, bk].
    // f is decreasing, so that f(ak) > 0 and f(bk) < 0.
    auto    ak = vmin;
    auto    bk = vmax;

	const auto  a = n + 1.0;
	const auto  b = Vddt;
	const auto  b_vi = ( b > vi ) ? ( b - vi ) : 0.0;
	const auto  c = n * ( b_vi * b_vi );

    for (;;)
    {
        const auto  xk = x;

        // Calculate f and df
        const auto	out = opamp->evaluate(x);
        const auto  vo = out.x;
        const auto  dvo = out.y;

		const auto  b_vx = ( b > x ) ? b - x : 0.;
		const auto  b_vo = ( b > vo ) ? b - vo : 0.;

		// f = a*(b - vx)^2 - c - (b - vo)^2
		const auto	f = a * ( b_vx * b_vx ) - c - ( b_vo * b_vo );

		// df = 2*((b - vo)*dvo - a*(b - vx))
		const auto	df = 2.0 * ( b_vo * dvo - a * b_vx );

        // Newton-Raphson step: xk1 = xk - f(xk)/f'(xk)
        x -= f / df;

		if ( std::fabs ( x - xk ) < EPSILON )
			return opamp->evaluate ( x ).x;

		// Narrow down root bracket
		if ( f < 0.0 )
			bk = xk;
		else
			ak = xk;

		// Bisection step (ala Dekker's method)
		if ( x <= ak || x >= bk )
			x = ( ak + bk ) * 0.5;
    }
}

} // namespace reSIDfp
