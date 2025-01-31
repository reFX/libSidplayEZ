#pragma once
/*
* This file is part of libsidplayfp, a SID player engine.
*
* Copyright 2012-2015 Leandro Nini <drfiemost@users.sourceforge.net>
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
#include <string>
#include <vector>

class SidTuneInfo;

namespace libsidplayfp
{

class sidmemory;

class psiddrv
{
private:
	const SidTuneInfo* m_tuneInfo;
	std::string	m_errorString;

	std::vector<uint8_t>	psid_driver;
	uint8_t* reloc_driver;
	int						reloc_size;

	uint16_t	m_driverAddr;
	uint16_t	m_driverLength;

	uint16_t	m_handshakeAddr;

private:
	/**
	* Get required I/O map to reach address
	*
	* @param addr a 16-bit effective address
	* @return a default bank-select value for $01
	*/
	[[ nodiscard ]] uint8_t iomap ( uint16_t addr ) const;

public:
	psiddrv ( const SidTuneInfo* tuneInfo )
		: m_tuneInfo ( tuneInfo )
	{
	}

	/**
	* Relocate the driver
	*
	* @return false if something is wrong, check #errorString for error details
	*/
	[[ nodiscard ]] bool drvReloc ();

	/**
	* Install the driver
	* Must be called after the tune has been placed in memory.
	*
	* @param mem the c64 memory interface
	* @param video the PAL/NTSC switch value, 0: NTSC, 1: PAL
	*/
	void install ( sidmemory& mem, uint8_t video );

	/**
	* Get a detailed error message.
	*
	* @return a pointer to the string
	*/
	[[ nodiscard ]] const char* errorString () const { return m_errorString.c_str (); }

	[[ nodiscard ]] uint16_t driverAddr () const { return m_driverAddr; }
	[[ nodiscard ]] uint16_t driverLength () const { return m_driverLength; }

	[[ nodiscard ]] uint16_t getHandshakeAddr () const { return m_handshakeAddr; }
};

}
