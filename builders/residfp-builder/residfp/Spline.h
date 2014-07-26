/*
 * This file is part of libsidplayfp, a SID player engine.
 *
 * Copyright 2011-2014 Leandro Nini <drfiemost@users.sourceforge.net>
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

#ifndef SPLINE_H
#define SPLINE_H

namespace reSIDfp
{

/**
 * Fritsch-Carlson monotone cubic spline interpolation.
 *
 * Based on the implementation from the [Monotone cubic interpolation] wikipedia page.
 *
 * [Monotone cubic interpolation]: https://en.wikipedia.org/wiki/Monotone_cubic_interpolation
 */
class Spline
{
public:
    typedef struct
    {
        double x;
        double y;
    } Point;

    typedef struct
    {
        double x1;
        double x2;
        double a;
        double b;
        double c;
        double d;
    } Param;

private:
    const int paramsLength;
    Param *params;
    Param *c;

private:
    /**
     * Calculate the slope of the line crossing the given points.
     */
    inline double slope(const Point &a, const Point &b);

public:
    Spline(const Point input[], int inputLength);
    ~Spline() { delete [] params; }

    /**
     * Evaluate y and its derivative at given point x.
     */
    void evaluate(double x, Point &out);
};

} // namespace reSIDfp

#endif
