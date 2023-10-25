#pragma once
/*
 * This file is part of libsidplayfp, a SID player engine.
 *
 * Copyright 2000 Simon White
 * Copyright 2023 Michael Hartmann
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

// Set the lo byte (8 bit) in a word (16 bit)
inline void endian_set16_lo8 ( uint16_t& word, uint8_t byte )
{
	word = ( word & 0xff00 ) | byte;
}
//-----------------------------------------------------------------------------

// Get the lo byte (8 bit) in a word (16 bit)
inline uint8_t endian_get16_lo8 ( uint16_t word )
{
	return uint8_t ( word );
}
//-----------------------------------------------------------------------------

// Set the hi byte (8 bit) in a word (16 bit)
inline void endian_set16_hi8 ( uint16_t& word, uint8_t byte )
{
	word = uint16_t ( ( word & 0x00ff ) | ( byte << 8 ) );
}
//-----------------------------------------------------------------------------

// Get the hi byte (8 bit) in a word (16 bit)
inline uint8_t endian_get16_hi8 ( uint16_t word )
{
	return word >> 8;
}
//-----------------------------------------------------------------------------

// Convert high-byte and low-byte to 16-bit word
inline uint16_t endian_get16 ( uint8_t hi, uint8_t lo )
{
	return uint16_t ( ( hi << 8 ) | lo );
}
//-----------------------------------------------------------------------------

// Convert high-byte and low-byte to 16-bit little endian word
inline uint16_t endian_getLittle16 ( const uint8_t ptr[ 2 ] )
{
	return uint16_t ( ( ptr[ 1 ] << 8 ) | ptr[ 0 ] );
}
//-----------------------------------------------------------------------------

// Write a little-endian 16-bit word to two bytes in memory
inline void endian_setLittle16 ( uint8_t ptr[ 2 ], uint16_t word )
{
	ptr[ 0 ] = uint8_t ( word );
	ptr[ 1 ] = uint8_t ( word >> 8 );
}
//-----------------------------------------------------------------------------

// Convert high-byte and low-byte to 16-bit big endian word
inline uint16_t endian_getBig16 ( const uint8_t ptr[ 2 ] )
{
	return uint16_t ( ( ptr[ 0 ] << 8 ) | ptr[ 1 ] );
}
//-----------------------------------------------------------------------------

// Get 32-bit big endian int from memory
inline uint32_t endian_getBig32 ( const uint8_t ptr[ 4 ] )
{
	return ( ptr[ 0 ] << 24 ) | ( ptr[ 1 ] << 16 ) | ( ptr[ 2 ] << 8 ) | ptr[ 3 ];
}
//-----------------------------------------------------------------------------
