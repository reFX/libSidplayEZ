#pragma once
/*
* This file is part of libsidplayfp, a SID player engine.
*
* Copyright 2011-2024 Leandro Nini <drfiemost@users.sourceforge.net>
* Copyright 2007-2010 Antti Lankila
* Copyright 2004,2010 Dag Lem
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

#include <memory>

#include "FilterModelConfig.h"
#include "Dac.h"

namespace reSIDfp
{

class Integrator6581;

/**
* Calculate parameters for 6581 filter emulation.
*/
class FilterModelConfig6581 final : public FilterModelConfig
{
private:
	// Transistor parameters.
	//@{
	const double WL_vcr;        ///< W/L for VCR
	const double WL_snake;      ///< W/L for "snake"
	//@}

	// DAC parameters.
	//@{
	const double dac_zero;
	const double dac_scale;
	//@}

	// DAC lookup table
	Dac	dac;

	// VCR - 6581 only.
	//@{
	uint16_t	vcr_nVg[ 1 << 16 ];
	double		vcr_n_Ids_term[ 1 << 16 ];
	//@}

	inline double getDacZero ( double adjustment ) const	{	return dac_zero + adjustment;	}

public:
	FilterModelConfig6581 ();
	~FilterModelConfig6581 () = default;

	static FilterModelConfig6581* getInstance ();

	void setFilterRange ( double adjustment );

	/**
	* Construct an 11 bit cutoff frequency DAC output voltage table.
	* Ownership is transferred to the requester which becomes responsible
	* of freeing the object when done.
	*
	* @param adjustment
	* @return the DAC table
	*/
	uint16_t* getDAC ( double adjustment ) const;
	double getWL_snake () const { return WL_snake; }

	inline uint16_t getVcr_nVg ( const int i ) const		{	return vcr_nVg[ i ];	}
	inline uint16_t getVcr_n_Ids_term ( const int i ) const	{	return uint16_t ( vcr_n_Ids_term[ i ] * uCox + 0.5 );	}

	#ifdef SLOPE_FACTOR
		inline double getUt () const { return Ut; }
		inline double getN16 () const { return N16; }
	#endif
};

} // namespace reSIDfp
