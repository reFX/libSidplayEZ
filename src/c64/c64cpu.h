#pragma once
/*
* This file is part of libsidplayfp, a SID player engine.
*
*  Copyright (C) 2012-2021 Leandro Nini
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "mmu.h"
#include "CPU/mos6510.h"

namespace libsidplayfp
{

//-----------------------------------------------------------------------------

class c64cpubus final : public CPUDataBus
{
public:
	c64cpubus ( MMU& mmu )
		: m_mmu ( mmu )
	{
	}

protected:
	uint8_t cpuRead ( uint16_t addr ) override				{	return m_mmu.cpuRead ( addr );	}
	void cpuWrite ( uint16_t addr, uint8_t data ) override	{	m_mmu.cpuWrite ( addr, data );	}

private:
	MMU&	m_mmu;
};
//-----------------------------------------------------------------------------

}
