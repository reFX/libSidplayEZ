/*
* This file is part of libsidplayfp, a SID player engine.
*
* Copyright 2011-2023 Leandro Nini <drfiemost@users.sourceforge.net>
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

#include "player.h"

#include "sidplayfp/SidTune.h"

#include "sidemu.h"
#include "psiddrv.h"
#include "romCheck.h"
#include "stringutils.h"

namespace libsidplayfp
{

/**
* Configuration error exception.
*/
class configError
{
private:
	const char* m_msg;

public:
	configError ( const char* msg ) : m_msg ( msg ) {}
	const char* message () const { return m_msg; }
};
//-----------------------------------------------------------------------------

Player::Player ()
{
	// We need at least some minimal interrupt handling
	m_c64.getMemInterface ().setKernal ( nullptr );

	setConfig ( m_cfg );

	// Get component credits
	m_info.m_credits.push_back ( m_c64.cpuCredits () );
	m_info.m_credits.push_back ( m_c64.ciaCredits () );
	m_info.m_credits.push_back ( m_c64.vicCredits () );
}
//-----------------------------------------------------------------------------

template<class T>
inline void checkRom ( const uint8_t* rom, std::string& desc )
{
	if ( rom )
	{
		T romCheck ( rom );
		desc.assign ( romCheck.info () );
	}
	else
		desc.clear ();
}
//-----------------------------------------------------------------------------

void Player::setKernal ( const uint8_t* rom )
{
	checkRom<kernalCheck> ( rom, m_info.m_kernalDesc );
	m_c64.getMemInterface ().setKernal ( rom );
}
//-----------------------------------------------------------------------------

void Player::setBasic ( const uint8_t* rom )
{
	checkRom<basicCheck> ( rom, m_info.m_basicDesc );
	m_c64.getMemInterface ().setBasic ( rom );
}
//-----------------------------------------------------------------------------

void Player::setChargen ( const uint8_t* rom )
{
	checkRom<chargenCheck> ( rom, m_info.m_chargenDesc );
	m_c64.getMemInterface ().setChargen ( rom );
}
//-----------------------------------------------------------------------------

void Player::initialise ()
{
	m_isPlaying = STOPPED;

	m_c64.reset ();

	const auto	tuneInfo = m_tune->getInfo ();

	const auto	size = uint32_t ( tuneInfo->loadAddr () ) + tuneInfo->c64dataLen () - 1;
	if ( size > 0xffff )
		throw configError ( "SIDPLAYER ERROR: Size of music data exceeds C64 memory." );

	psiddrv	driver ( m_tune->getInfo () );
	driver.powerOnDelay ( 32767 );// SidConfig::MAX_POWER_ON_DELAY );
	if ( ! driver.drvReloc () )
		throw configError ( driver.errorString () );

	m_info.m_driverAddr = driver.driverAddr ();
	m_info.m_driverLength = driver.driverLength ();
	m_info.m_powerOnDelay = SidConfig::MAX_POWER_ON_DELAY;

	driver.install ( m_c64.getMemInterface (), videoSwitch );

	if ( ! m_tune->placeSidTuneInC64mem ( m_c64.getMemInterface () ) )
		throw configError ( m_tune->statusString () );

	m_c64.resetCpu ();
}
//-----------------------------------------------------------------------------

bool Player::loadTune ( SidTune* tune )
{
	if ( m_tune = tune; tune )
	{
		// Must re-configure on fly for stereo support!
		if ( ! setConfig ( m_cfg, true ) )
		{
			// Failed configuration with new tune, reject it
			m_tune = nullptr;
			return false;
		}
	}

	return true;
}
//-----------------------------------------------------------------------------

uint32_t Player::play ( int16_t* buffer, uint32_t count )
{
	// Make sure a tune is loaded
	if ( ! m_tune )
		return 0;

	// Start the player loop
	if ( m_isPlaying == STOPPED )
		m_isPlaying = PLAYING;

	if ( m_isPlaying == PLAYING )
	{
		m_mixer.init ( buffer, count );

		if ( m_mixer.getSid ( 0 ) )
		{
			if ( count && buffer )
			{
				// Clock chips and mix into output buffer
				while ( m_mixer.notFinished () )
				{
					run ( sidemu::OUTPUTBUFFERSIZE );

					m_mixer.clockChips ();
					m_mixer.doMix ();
				}
				count = m_mixer.samplesGenerated ();
			}
			else
			{
				// Clock chips and discard buffers
				auto	size = int ( m_c64.getMainCpuSpeed () / m_cfg.frequency );
				while ( --size )
				{
					run ( sidemu::OUTPUTBUFFERSIZE );

					m_mixer.clockChips ();
					m_mixer.resetBufs ();
				}
			}
		}
		else
		{
			// Clock the machine
			auto	size = int ( m_c64.getMainCpuSpeed () / m_cfg.frequency );
			while ( --size )
				run ( sidemu::OUTPUTBUFFERSIZE );
		}
	}

	if ( m_isPlaying == STOPPING )
	{
		try
		{
			initialise ();
		}
		catch ( configError const& ) {}
		m_isPlaying = STOPPED;
	}

	return count;
}
//-----------------------------------------------------------------------------

void Player::stop ()
{
	if ( m_tune && m_isPlaying == PLAYING )
		m_isPlaying = STOPPING;
}
//-----------------------------------------------------------------------------

bool Player::setConfig ( const SidConfig& cfg, bool force )
{
	// Check if configuration have been changed or forced
	if ( ! force && ! m_cfg.compare ( cfg ) )
		return true;

	// Check for base sampling frequency
	if ( cfg.frequency < 8000 )
	{
		m_errorString = "SIDPLAYER ERROR: Unsupported sampling frequency.";
		return false;
	}

	// Only do these if we have a loaded tune
	if ( m_tune )
	{
		const auto	tuneInfo = m_tune->getInfo ();

		try
		{
			sidRelease ();

			std::vector<uint16_t>	addresses = { 0xD400 };	// First SID chip is always at $D400

			auto addSid = [ &addresses, tuneInfo ] ( const auto sidIndex, uint16_t fallbackAddr )
			{
				if ( auto newSidAddress = tuneInfo->sidChipBase ( sidIndex ) )
				{
					addresses.push_back ( newSidAddress );
					return;
				}

				if ( fallbackAddr )
					addresses.push_back ( fallbackAddr );
			};

			addSid ( 1, cfg.secondSidAddress );
			addSid ( 2, cfg.thirdSidAddress );

			// SID emulation setup (must be performed before the environment setup call)
			sidCreate ( cfg.defaultSidModel, cfg.forceSidModel, addresses );

			//
			// Attempt to have better sounding SIDs by adjusting filter-range, digi-boost, and combined waveform strength
			// per author with the assumption they worked with the same machine their entire career
			//
			const auto [ profileName, chipProfile ] = chipSelector.getChipProfile ( tuneInfo->path (), tuneInfo->dataFileName () );

			selectedChipProfile = profileName;

			setCombinedWaveforms ( reSIDfp::CombinedWaveforms ( chipProfile.cwsLevel ), float ( chipProfile.cwsThreshold ) );
			set6581FilterRange ( chipProfile.filter );
			set6581FilterCurve ( chipProfile.zeroDac );
			set6581DigiVolume ( chipProfile.digi );

			m_c64.setModel ( c64model ( cfg.defaultC64Model, cfg.forceC64Model ) );

			auto getCiaModel = [] ( SidConfig::cia_model_t model )
			{
				switch ( model )
				{
					default:
					case SidConfig::MOS6526:		return c64::OLD;
					case SidConfig::MOS8521:		return c64::NEW;
					case SidConfig::MOS6526W4485:	return c64::OLD_4485;
				}
			};
			m_c64.setCiaModel ( getCiaModel ( cfg.ciaModel ) );

			sidParams ( m_c64.getMainCpuSpeed (), cfg.frequency );

			// Configure, setup and install C64 environment/events
			initialise ();
		}
		catch ( configError const& e )
		{
			m_errorString = e.message ();

			if ( &m_cfg != &cfg )
				setConfig ( m_cfg );

			return false;
		}
	}

	const auto	isStereo = cfg.playback == SidConfig::STEREO;
	m_info.m_channels = isStereo ? 2 : 1;

	m_mixer.setStereo ( isStereo );
	m_mixer.setSamplerate ( cfg.frequency );

	// Update Configuration
	m_cfg = cfg;

	return true;
}
//-----------------------------------------------------------------------------

// Clock speed changes due to loading a new song
c64::model_t Player::c64model ( SidConfig::c64_model_t defaultModel, bool forced )
{
	const auto	tuneInfo = m_tune->getInfo ();

	auto	clockSpeed = tuneInfo->clockSpeed ();

	c64::model_t model;

	// Use preferred speed if forced or if song speed is unknown
	if ( forced || clockSpeed == SidTuneInfo::CLOCK_UNKNOWN || clockSpeed == SidTuneInfo::CLOCK_ANY )
	{
		switch ( defaultModel )
		{
			case SidConfig::PAL:
				clockSpeed = SidTuneInfo::CLOCK_PAL;
				model = c64::PAL_B;
				videoSwitch = 1;
				break;

			case SidConfig::DREAN:
				clockSpeed = SidTuneInfo::CLOCK_PAL;
				model = c64::PAL_N;
				videoSwitch = 1; // TODO verify
				break;

			case SidConfig::NTSC:
				clockSpeed = SidTuneInfo::CLOCK_NTSC;
				model = c64::NTSC_M;
				videoSwitch = 0;
				break;

			case SidConfig::OLD_NTSC:
				clockSpeed = SidTuneInfo::CLOCK_NTSC;
				model = c64::OLD_NTSC_M;
				videoSwitch = 0;
				break;

			case SidConfig::PAL_M:
				clockSpeed = SidTuneInfo::CLOCK_NTSC;
				model = c64::PAL_M;
				videoSwitch = 0; // TODO verify
				break;
		}
	}
	else
	{
		switch ( clockSpeed )
		{
			default:
			case SidTuneInfo::CLOCK_PAL:
				model = c64::PAL_B;
				videoSwitch = 1;
				break;

			case SidTuneInfo::CLOCK_NTSC:
				model = c64::NTSC_M;
				videoSwitch = 0;
				break;
		}
	}

	switch ( clockSpeed )
	{
		case SidTuneInfo::CLOCK_PAL:
			if ( tuneInfo->songSpeed () == SidTuneInfo::SPEED_CIA_1A )
				m_info.m_speedString = "CIA (PAL)";
			else if ( tuneInfo->clockSpeed () == SidTuneInfo::CLOCK_NTSC )
				m_info.m_speedString = "60 Hz VBI (PAL FIXED)";
			else
				m_info.m_speedString = "50 Hz VBI (PAL)";
			break;

		case SidTuneInfo::CLOCK_NTSC:
			if ( tuneInfo->songSpeed () == SidTuneInfo::SPEED_CIA_1A )
				m_info.m_speedString = "CIA (NTSC)";
			else if ( tuneInfo->clockSpeed () == SidTuneInfo::CLOCK_PAL )
				m_info.m_speedString = "50 Hz VBI (NTSC FIXED)";
			else
				m_info.m_speedString = "60 Hz VBI (NTSC)";
			break;

		default:
			break;
	}

	return model;
}
//-----------------------------------------------------------------------------

void Player::sidRelease ()
{
	m_c64.clearSids ();

	for ( auto i = 0; i < 3; i++ )
		if ( auto s = m_mixer.getSid ( i ) )
				s->unlock ();

	m_mixer.clearSids ();
}
//-----------------------------------------------------------------------------

void Player::sidCreate ( SidConfig::sid_model_t defaultModel, bool forced, const std::vector<uint16_t>& sidAddresses )
{
	const auto  tuneInfo = m_tune->getInfo ();

	auto getSidModel = [] ( const SidTuneInfo::model_t sidModel, const SidConfig::sid_model_t _defaultModel, const bool _forced )
	{
		// Use preferred SID model if forced or if song SID model is unknown
		if ( _forced || sidModel == SidTuneInfo::SIDMODEL_UNKNOWN || sidModel == SidTuneInfo::SIDMODEL_ANY )
			return _defaultModel;

		return sidModel == SidTuneInfo::SIDMODEL_6581 ? SidConfig::MOS6581 : SidConfig::MOS8580;
	};

	for ( auto i = 0; auto extraAddr : sidAddresses )
	{
		defaultModel = getSidModel ( tuneInfo->sidModel ( i ), defaultModel, forced );

		auto	s = &m_sidEmu[ i ];
		s->lock ( m_c64.getEventScheduler () );
		s->model ( defaultModel );

		if ( i++ == 0 )
			m_c64.setBaseSid ( s );
		else if ( ! m_c64.addExtraSid ( s, extraAddr ) )
			throw configError ( "SIDPLAYER ERROR: Unsupported SID address." );

		m_mixer.addSid ( s );
	}
}
//-----------------------------------------------------------------------------

void Player::sidParams ( double cpuFreq, int frequency )
{
	for ( auto i = 0; i < 3 ; i++ )
		if ( auto s = m_mixer.getSid ( i ) )
			s->sampling ( float ( cpuFreq ), frequency );
}
//-----------------------------------------------------------------------------

void Player::setCombinedWaveforms ( reSIDfp::CombinedWaveforms cws, const float threshold )
{
	for ( auto i = 0; i < 3; i++ )
		if ( auto s = m_mixer.getSid ( i ) )
			s->combinedWaveforms ( cws, threshold );
}
//-----------------------------------------------------------------------------

void Player::set6581FilterCurve ( const double value )
{
	for ( auto i = 0; i < 3; i++ )
		if ( auto s = m_mixer.getSid ( i ) )
			s->filter6581Curve ( value );
}
//-----------------------------------------------------------------------------

void Player::set6581FilterRange ( const double value )
{
	for ( auto i = 0; i < 3; i++ )
		if ( auto s = m_mixer.getSid ( i ) )
			s->filter6581Range ( value );
}
//-----------------------------------------------------------------------------

void Player::set6581DigiVolume ( const double value )
{
	for ( auto i = 0; i < 3; i++ )
		if ( auto s = m_mixer.getSid ( i ) )
			s->filter6581Digi ( value );
}
//-----------------------------------------------------------------------------

void Player::setDacLeakage ( const double value )
{
	for ( auto i = 0; i < 3; i++ )
		if ( auto s = m_mixer.getSid ( i ) )
			s->setDacLeakage ( value );
}
//-----------------------------------------------------------------------------

bool Player::getSidStatus ( int sidNum, uint8_t regs[ 32 ] )
{
	if ( auto s = m_mixer.getSid ( sidNum ) )
	{
		s->getStatus ( regs );

		// Write envelope-levels into unused SID registers
		regs[ 0x1d ] = uint8_t ( s->getInternalEnvValue ( 0 ) * 255.0f );
		regs[ 0x1e ] = uint8_t ( s->getInternalEnvValue ( 1 ) * 255.0f );
		regs[ 0x1f ] = uint8_t ( s->getInternalEnvValue ( 2 ) * 255.0f );

		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------

}
