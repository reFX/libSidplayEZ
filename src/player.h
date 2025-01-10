#pragma once
/*
* This file is part of libsidplayfp, a SID player engine.
*
* Copyright 2011-2022 Leandro Nini <drfiemost@users.sourceforge.net>
* Copyright 2007-2010 Antti Lankila
* Copyright 2000-2001 Simon White
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
#include <cstdio>

#include "sidplayfp/SidConfig.h"
#include "sidplayfp/SidTune.h"
#include "sidplayfp/SidTuneInfo.h"
#include "SidInfoImpl.h"
#include "sidemu.h"

#include "mixer.h"
#include "c64/c64.h"

#include "EZ/chip-selector.h"

#include <atomic>
#include <vector>
#include <unordered_map>

namespace libsidplayfp
{

class Player final
{
private:
	enum class state_t
	{
		STOPPED,
		PLAYING,
		STOPPING
	};

	c64			m_c64;				// Commodore 64 emulator
	Mixer		m_mixer;			// Mixer
	SidTune*	m_tune = nullptr;	// Emulator info
	SidInfoImpl	m_info;				// Tune info
	SidConfig	m_cfg;				// User Configuration Settings

	sidemu		m_sidEmu[ 3 ] = { m_c64.getEventScheduler (), m_c64.getEventScheduler (),m_c64.getEventScheduler () };		// emulation of an actual SID chip

	std::string	m_errorString = "N/A";

	state_t		m_isPlaying = state_t::STOPPED;	// Playback status
	uint8_t		videoSwitch;					// PAL/NTSC switch value

	/**
	* Get the C64 model for the current loaded tune.
	*
	* @param defaultModel the default model
	* @param forced true if the default model should be forced in spite of tune model
	*/
	c64::model_t c64model ( SidConfig::c64_model_t defaultModel, bool forced );

	void initialise ();

	void sidRelease ();
	void sidCreate ( SidConfig::sid_model_t defaultModel, bool forced, const std::vector<uint16_t>& sidAddresses );

	void sidParams ( double cpuFreq, int frequency );

	sidinline void run ( unsigned int events )
	{
		while ( events-- )
			m_c64.clock ();
	}

public:
	Player ();

	bool setConfig ( const SidConfig& cfg, bool force = false );
	[[ nodiscard ]] const SidConfig& getConfig () const { return m_cfg; }

	[[ nodiscard ]] const SidInfo& getInfo () const { return m_info; }

	bool loadTune ( SidTune* tune );
	uint32_t play ( int16_t* buffer, uint32_t samples );
	void stop ();
	[[ nodiscard ]] bool isPlaying () const { return m_isPlaying != state_t::STOPPED; }

	[[ nodiscard ]] int getNumChips () const { return m_mixer.getNumChips (); }

	void setCombinedWaveforms ( reSIDfp::CombinedWaveforms cws, const float threshold );

	void set6581FilterCurve ( const double value );
	void set6581FilterRange ( const double value );
	void set6581FilterGain ( const double value );
	void set6581DigiVolume ( const double value );

	void setDacLeakage ( const double value );
	void set6581VoiceDCDrift ( const double value );

	[[ nodiscard ]] uint32_t time () const { return m_c64.getTimeMs () / 1000; }		// Time in seconds
	[[ nodiscard ]] uint32_t timeMs () const { return m_c64.getTimeMs (); }				// Time in milliseconds

	[[ nodiscard ]] const char* error () const { return m_errorString.c_str (); }

	void setKernal ( const uint8_t* rom );
	void setBasic ( const uint8_t* rom );
	void setChargen ( const uint8_t* rom );

	[[ nodiscard ]] uint16_t getCia1TimerA () const { return m_c64.getCia1TimerA (); }

	bool getSidStatus ( int sidNum, uint8_t regs[ 32 ] );
};

}
