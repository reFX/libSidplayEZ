/*
 * This file is part of libsidplayfp, a SID player engine.
 *
 * Copyright 2011-2020 Leandro Nini <drfiemost@users.sourceforge.net>
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

#include "SincResampler.h"

#include <cassert>
#include <cstring>
#include <cmath>
#include <map>
#include <iostream>
#include <sstream>

constexpr auto	M_PI = 3.14159265358979323846;

//
// SSE2 detection
//
#if (defined(_M_AMD64) || defined(_M_X64)) || __SSE2__
	#define HAVE_SSE2
	#include <emmintrin.h>
#elif (defined(__arm64__) && defined(__APPLE__)) || defined (__aarch64__)
	#define HAVE_NEON
	#include <arm_neon.h>
#endif

#include <mutex>

namespace reSIDfp
{

typedef std::map<std::string, matrix_t> fir_cache_t;

// Cache for the expensive FIR table computation results
fir_cache_t FIR_CACHE;
std::mutex FIR_CACHE_Lock;

// Maximum error acceptable in I0 is 1e-6, or ~96 dB
constexpr auto	I0E = 1e-6;
constexpr auto	BITS = 16;

//-----------------------------------------------------------------------------

/**
* Compute the 0th order modified Bessel function of the first kind.
* This function is originally from resample-1.5/filterkit.c by J. O. Smith.
* It is used to build the Kaiser window for resampling.
*
* @param x evaluate I0 at x
* @return value of I0 at x.
*/
double I0 ( double x )
{
	auto	sum = 1.0;
	auto	u = 1.0;
	auto	n = 1.0;
	const auto	halfx = x / 2.0;

	do
	{
		const auto	temp = halfx / n;

		u *= temp * temp;
		sum += u;
		n += 1.0;

	} while ( u >= I0E * sum );

	return sum;
}
//-----------------------------------------------------------------------------

/**
* Calculate convolution with sample and sinc.
*
* @param a sample buffer input
* @param b sinc buffer
* @param bLength length of the sinc buffer
* @return convolved result
*/
static int convolve ( const short* a, const short* b, int bLength )
{
	#ifdef HAVE_SSE2
	//
	// SSE2 version
	//
	auto    out = 0;

	const auto  offset = (uintptr_t)( a ) & 0x0f;

	// check for aligned accesses
	if ( offset == ( (uintptr_t)( b ) & 0x0f ) )
	{
		if ( offset )
		{
			const auto  l = int ( ( 0x10 - offset ) / 2 );

			for ( auto i = 0; i < l; i++ )
				out += *a++ * *b++;

			bLength -= offset;
		}

		auto    acc = _mm_setzero_si128 ();

		const auto  n = bLength / 8;

		for ( auto i = 0; i < n; i++ )
		{
			const auto  tmp = _mm_madd_epi16 ( *( __m128i* )a, *( __m128i* )b );
			acc = _mm_add_epi16 ( acc, tmp );
			a += 8;
			b += 8;
		}

		auto    vsum = _mm_add_epi32 ( acc, _mm_srli_si128 ( acc, 8 ) );
		vsum = _mm_add_epi32 ( vsum, _mm_srli_si128 ( vsum, 4 ) );
		out += _mm_cvtsi128_si32 ( vsum );

		bLength &= 7;
	}
	#elif defined(HAVE_NEON)
	//
	// NEON version
	//
	int32x4_t acc1Low = vdupq_n_s32 ( 0 );
	int32x4_t acc1High = vdupq_n_s32 ( 0 );
	int32x4_t acc2Low = vdupq_n_s32 ( 0 );
	int32x4_t acc2High = vdupq_n_s32 ( 0 );

	const int n = bLength / 16;

	for ( int i = 0; i < n; i++ )
	{
		int16x8_t v11 = vld1q_s16 ( a );
		int16x8_t v12 = vld1q_s16 ( a + 8 );
		int16x8_t v21 = vld1q_s16 ( b );
		int16x8_t v22 = vld1q_s16 ( b + 8 );

		acc1Low = vmlal_s16 ( acc1Low, vget_low_s16 ( v11 ), vget_low_s16 ( v21 ) );
		acc1High = vmlal_high_s16 ( acc1High, v11, v21 );
		acc2Low = vmlal_s16 ( acc2Low, vget_low_s16 ( v12 ), vget_low_s16 ( v22 ) );
		acc2High = vmlal_high_s16 ( acc2High, v12, v22 );

		a += 16;
		b += 16;
	}

	bLength &= 15;

	if ( bLength >= 8 )
	{
		int16x8_t v1 = vld1q_s16 ( a );
		int16x8_t v2 = vld1q_s16 ( b );

		acc1Low = vmlal_s16 ( acc1Low, vget_low_s16 ( v1 ), vget_low_s16 ( v2 ) );
		acc1High = vmlal_high_s16 ( acc1High, v1, v2 );

		a += 8;
		b += 8;
	}

	bLength &= 7;

	if ( bLength >= 4 )
	{
		int16x4_t v1 = vld1_s16 ( a );
		int16x4_t v2 = vld1_s16 ( b );

		acc1Low = vmlal_s16 ( acc1Low, v1, v2 );

		a += 4;
		b += 4;
	}

	int32x4_t accSumsNeon = vaddq_s32 ( acc1Low, acc1High );
	accSumsNeon = vaddq_s32 ( accSumsNeon, acc2Low );
	accSumsNeon = vaddq_s32 ( accSumsNeon, acc2High );

	int out = vaddvq_s32 ( accSumsNeon );

	bLength &= 3;
	#endif

	for ( auto i = 0; i < bLength; i++ )
		out += *a++ * *b++;

	return ( out + ( 1 << 14 ) ) >> 15;
}
//-----------------------------------------------------------------------------

int SincResampler::fir ( int subcycle )
{
	// Find the first of the nearest fir tables close to the phase
	auto		firTableFirst = ( subcycle * firRES >> 10 );
	const auto	firTableOffset = ( subcycle * firRES ) & 0x3ff;

	// Find firN most recent samples, plus one extra in case the FIR wraps.
	auto	sampleStart = sampleIndex - firN + RINGSIZE - 1;

	const auto	v1 = convolve ( sample + sampleStart, ( *firTable )[ firTableFirst ], firN );

	// Use next FIR table, wrap around to first FIR table using
	// previous sample.
	if ( ++firTableFirst == firRES )
	{
		firTableFirst = 0;
		++sampleStart;
	}

	const auto	v2 = convolve ( sample + sampleStart, ( *firTable )[ firTableFirst ], firN );

	// Linear interpolation between the sinc tables yields good
	// approximation for the exact value.
	return v1 + ( firTableOffset * ( v2 - v1 ) >> 10 );
}
//-----------------------------------------------------------------------------

SincResampler::SincResampler ( double clockFrequency, double samplingFrequency, double highestAccurateFrequency )
	: cyclesPerSample ( static_cast<int>( clockFrequency / samplingFrequency * 1024. ) )
{
	// 16 bits -> -96dB stopband attenuation.
	const auto	A = -20.0 * std::log10 ( 1.0 / ( 1 << BITS ) );
	// A fraction of the bandwidth is allocated to the transition band, which we double
	// because we design the filter to transition halfway at nyquist.
	const auto	dw = ( 1.0 - 2.0 * highestAccurateFrequency / samplingFrequency ) * M_PI * 2.0;

	// For calculation of beta and N see the reference for the kaiserord
	// function in the MATLAB Signal Processing Toolbox:
	// http://www.mathworks.com/help/signal/ref/kaiserord.html
	const auto	beta = 0.1102 * ( A - 8.7 );
	const auto	I0beta = I0 ( beta );
	const auto	cyclesPerSampleD = clockFrequency / samplingFrequency;

	{
		// The filter order will maximally be 124 with the current constraints.
		// N >= (96.33 - 7.95)/(2 * pi * 2.285 * (maxfreq - passbandfreq) >= 123
		// The filter order is equal to the number of zero crossings, i.e.
		// it should be an even number (sinc is symmetric with respect to x = 0).
		auto	N = int ( ( A - 7.95 ) / ( 2.285 * dw ) + 0.5 );
		N += N & 1;

		// The filter length is equal to the filter order + 1.
		// The filter length must be an odd number (sinc is symmetric with respect to
		// x = 0).
		firN = int ( ( N * cyclesPerSampleD ) + 1 ) | 1;

		// Check whether the sample ring buffer would overflow.
		assert ( firN < RINGSIZE );

		// Error is bounded by err < 1.234 / L^2, so L = sqrt(1.234 / (2^-16)) = sqrt(1.234 * 2^16).
		firRES = int ( std::ceil ( std::sqrt ( 1.234 * ( 1 << BITS ) ) / cyclesPerSampleD ) );

		// firN*firRES represent the total resolution of the sinc sampling. JOS
		// recommends a length of 2^BITS, but we don't quite use that good a filter.
		// The filter test program indicates that the filter performs well, though.
	}

	// Create the map key
	std::ostringstream o;
	o << firN << "," << firRES << "," << cyclesPerSampleD;
	const std::string firKey = o.str ();

	std::lock_guard<std::mutex> lock ( FIR_CACHE_Lock );

	fir_cache_t::iterator lb = FIR_CACHE.lower_bound ( firKey );

	// The FIR computation is expensive and we set sampling parameters often, but
	// from a very small set of choices. Thus, caching is used to speed initialization.
	if ( lb != FIR_CACHE.end () && !( FIR_CACHE.key_comp ()( firKey, lb->first ) ) )
	{
		firTable = &( lb->second );
	}
	else
	{
		// Allocate memory for FIR tables.
		matrix_t tempTable ( firRES, firN );

		firTable = &( FIR_CACHE.emplace_hint ( lb, fir_cache_t::value_type ( firKey, tempTable ) )->second );

		// The cutoff frequency is midway through the transition band, in effect the same as nyquist.
		const auto	wc = M_PI;

		// Calculate the sinc tables.
		const auto	scale = 32768.0 * wc / cyclesPerSampleD / M_PI;

		// we're not interested in the fractional part
		// so use int division before converting to double
		const auto	tmp = firN / 2;
		const auto	firN_2 = double ( tmp );

		for ( auto i = 0; i < firRES; i++ )
		{
			const auto	jPhase = double ( i ) / firRES + firN_2;

			for ( auto j = 0; j < firN; j++ )
			{
				const auto	x = j - jPhase;

				const auto	xt = x / firN_2;
				const auto	kaiserXt = std::fabs ( xt ) < 1. ? I0 ( beta * std::sqrt ( 1. - xt * xt ) ) / I0beta : 0.;

				const auto	wt = wc * x / cyclesPerSampleD;
				const auto	sincWt = std::fabs ( wt ) >= 1e-8 ? std::sin ( wt ) / wt : 1.;

				( *firTable )[ i ][ j ] = short ( scale * sincWt * kaiserXt );
			}
		}
	}
}
//-----------------------------------------------------------------------------

bool SincResampler::input ( int input )
{
	auto	ready = false;

	/*
	* Clip the input as it may overflow the 16 bit range.
	*
	* Approximate measured input ranges:
	* 6581: [-24262,+25080]  (Kawasaki_Synthesizer_Demo)
	* 8580: [-21514,+35232]  (64_Forever, Drum_Fool)
	*/
	auto softClip = [] ( int x )
	{
		constexpr auto	threshold = 28000;
		if ( x < threshold )
			return short ( x );

		constexpr auto	t = threshold / 32768.0;
		constexpr auto	a = 1.0 - t;
		constexpr auto	b = 1.0 / a;

		auto	value = double ( x - threshold ) / 32768.0;
		value = t + a * std::tanh ( b * value );

		return short ( value * 32768.0 );
	};

	sample[ sampleIndex ] = sample[ sampleIndex + RINGSIZE ] = softClip ( input );
	sampleIndex = ( sampleIndex + 1 ) & ( RINGSIZE - 1 );

	if ( sampleOffset < 1024 )
	{
		outputValue = fir ( sampleOffset );
		ready = true;
		sampleOffset += cyclesPerSample;
	}

	sampleOffset -= 1024;

	return ready;
}
//-----------------------------------------------------------------------------

void SincResampler::reset ()
{
	std::fill_n ( sample, std::size ( sample ), 0 );
	sampleOffset = 0;
}
//-----------------------------------------------------------------------------

} // namespace reSIDfp
