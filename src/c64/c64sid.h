#pragma once
/*
 * This file is part of libsidplayfp, a SID player engine.
 *
 * Copyright 2013-2021 Leandro Nini <drfiemost@users.sourceforge.net>
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

#include "Banks/Bank.h"

#include <cstring>
#include <stdint.h>

namespace libsidplayfp
{

/**
 * SID interface.
 */
class c64sid : public Bank
{
private:
	uint8_t	lastpoke[ 0x20 ] = {};

protected:
	virtual uint8_t read ( uint8_t addr ) = 0;
	virtual void write ( uint8_t addr, uint8_t data ) = 0;

public:
	virtual void reset ( uint8_t volume ) = 0;

	void reset ()	{	std::fill_n ( lastpoke, std::size ( lastpoke ), 0 );	}

	// Bank functions
	void poke ( uint16_t address, uint8_t value ) override
	{
		lastpoke[ address & 0x1f ] = value;
		write ( address & 0x1f, value );
	}
	uint8_t peek ( uint16_t address ) override { return read ( address & 0x1f ); }

	void getStatus ( uint8_t regs[ 0x20 ] ) const { std::copy_n ( lastpoke, std::size ( lastpoke ), regs ); }
};

}
