#pragma once
/*
 * This file is part of libsidplayfp, a SID player engine.
 *
 *  Copyright 2011-2019 Leandro Nini
 *  Copyright 2007-2010 Antti Lankila
 *  Copyright 2000 Simon White
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

#include <stdint.h>

 /**
  * This interface is used to get sid engine information
  */
class SidInfo
{
public:
	/// Library name
	const char* name () const { return getName (); }

	/// Library version
	const char* version () const { return getVersion (); }

	/// Library credits
	//@{
	unsigned int numberOfCredits () const { return getNumberOfCredits (); }
	const char* credits ( unsigned int i ) const { return getCredits ( i ); }
	//@}

	/// Number of SIDs supported by this library
	unsigned int maxsids () const { return getMaxsids (); }

	/// Number of output channels (1-mono, 2-stereo)
	unsigned int channels () const { return getChannels (); }

	/// Address of the driver
	uint16_t driverAddr () const { return getDriverAddr (); }

	/// Size of the driver in bytes
	uint16_t driverLength () const { return getDriverLength (); }

	/// Power on delay
	uint16_t powerOnDelay () const { return getPowerOnDelay (); }

	/// Describes the speed current song is running at
	const char* speedString () const { return getSpeedString (); }

	/// Description of the loaded ROM images
	//@{
	const char* kernalDesc () const { return getKernalDesc (); }
	const char* basicDesc () const { return getBasicDesc (); }
	const char* chargenDesc () const { return getChargenDesc (); }
	//@}

private:
	virtual const char* getName () const = 0;

	virtual const char* getVersion () const = 0;

	virtual unsigned int getNumberOfCredits () const = 0;
	virtual const char* getCredits ( unsigned int i ) const = 0;

	virtual unsigned int getMaxsids () const = 0;

	virtual unsigned int getChannels () const = 0;

	virtual uint16_t getDriverAddr () const = 0;

	virtual uint16_t getDriverLength () const = 0;

	virtual uint16_t getPowerOnDelay () const = 0;

	virtual const char* getSpeedString () const = 0;

	virtual const char* getKernalDesc () const = 0;
	virtual const char* getBasicDesc () const = 0;
	virtual const char* getChargenDesc () const = 0;
};
//-----------------------------------------------------------------------------
