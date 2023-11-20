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

#include "c64env.h"
#include "CPU/mos6510.h"

namespace libsidplayfp
{

//-----------------------------------------------------------------------------

class c64cpu final : public MOS6510
{
public:
	c64cpu ( c64env& env )
		: MOS6510 ( env.scheduler () )
		, m_env ( env )
	{
	}

protected:
	uint8_t cpuRead ( uint16_t addr ) override				{	return m_env.cpuRead ( addr );	}
	void cpuWrite ( uint16_t addr, uint8_t data ) override	{	m_env.cpuWrite ( addr, data );	}

private:
	c64env&	m_env;
};
//-----------------------------------------------------------------------------

}
