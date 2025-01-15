#pragma once
/*
 * This file is part of libsidplayfp, a SID player engine.
 *
 * Copyright 2011-2014 Leandro Nini <drfiemost@users.sourceforge.net>
 * Copyright 2009-2014 VICE Project
 * Copyright 2007-2010 Antti Lankila
 * Copyright 2001 Simon White
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

#include <stdint.h>

#include <algorithm>
#include <iterator>

#include "../../EZ/config.h"

constexpr auto  SPRITES = 8;

namespace libsidplayfp
{

/**
 * Sprites handling.
 */
class Sprites
{
private:
    const uint8_t &enable, &y_expansion;

	uint8_t exp_flop;
	uint8_t dma;
	uint8_t mc_base[ SPRITES ];
	uint8_t mc[ SPRITES ];

public:
	Sprites ( uint8_t regs[ 0x40 ] )
		: enable ( regs[ 0x15 ] )
		, y_expansion ( regs[ 0x17 ] )
    {
		reset ();
	}

	sidinline void reset ()
	{
		exp_flop = 0xff;
		dma = 0;

		std::fill ( std::begin ( mc_base ), std::end ( mc_base ), 0 );
		std::fill ( std::begin ( mc ), std::end ( mc ), 0 );
	}

    /**
     * Update mc values in one pass
     * after the dma has been processed
     */
	sidinline void updateMc ()
	{
		uint8_t mask = 1;
		for ( auto i = 0u; i < SPRITES; i++, mask <<= 1 )
			if ( dma & mask )
				mc[ i ] = ( mc[ i ] + 3 ) & 0x3f;
	}

    /**
     * Update mc base value.
     */
	sidinline void updateMcBase()
    {
		uint8_t mask = 1;
		for ( auto i = 0u; i < SPRITES; i++, mask <<= 1 )
		{
			if ( exp_flop & mask )
			{
				mc_base[ i ] = mc[ i ];
				if ( mc_base[ i ] == 0x3f )
					dma &= ~mask;
			}
		}
    }

    /**
     * Calculate sprite expansion.
     */
	sidinline void checkExp()
    {
        exp_flop ^= dma & y_expansion;
    }

    /**
     * Check if sprite is displayed.
     */
	sidinline void checkDisplay()
    {
        std::copy_n ( mc_base, SPRITES, mc );
    }

    /**
     * Calculate sprite DMA.
     *
     * @rasterY y raster position
     * @regs the VIC registers
     */
	sidinline void checkDma ( unsigned int rasterY, uint8_t regs[ 0x40 ] )
	{
		const uint8_t y = rasterY & 0xff;
		uint8_t mask = 1;
		for ( auto i = 0u; i < SPRITES; i++, mask <<= 1 )
		{
			if ( ( enable & mask ) && ( y == regs[ ( i << 1 ) + 1 ] ) && !( dma & mask ) )
			{
				dma |= mask;
				mc_base[ i ] = 0;
				exp_flop |= mask;
			}
		}
	}

    /**
     * Calculate line crunch.
     *
     * @param data the data written to the register
     * @param lineCycle current line cycle
     */
	sidinline void lineCrunch ( uint8_t data, unsigned int lineCycle )
	{
		uint8_t mask = 1;
		for ( auto i = 0u; i < SPRITES; i++, mask <<= 1 )
		{
			if ( ! ( data & mask ) && ! ( exp_flop & mask ) )
			{
				// sprite crunch
				if ( lineCycle == 14 )
				{
					const auto	mc_i = mc[ i ];
					const auto	mcBase_i = mc_base[ i ];

					mc[ i ] = ( 0x2a & ( mcBase_i & mc_i ) ) | ( 0x15 & ( mcBase_i | mc_i ) );

					// mcbase will be set from mc on the following clock call
				}

				exp_flop |= mask;
			}
		}
	}

    /**
     * Check if dma is active for sprites.
     *
     * @param val bitmask for selected sprites
     */
	[[ nodiscard ]] sidinline bool isDma ( unsigned int val ) const
	{
		return dma & val;
	}
};

}
