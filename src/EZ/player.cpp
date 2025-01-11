#include "player.h"

#include "../stringutils.h"

//-----------------------------------------------------------------------------

namespace libsidplayEZ
{

//-----------------------------------------------------------------------------

void Player::setRoms ( const void* kernal, const void* basic, const void* character )
{
	engine.setKernal ( (const uint8_t*)kernal );
	engine.setBasic ( (const uint8_t*)basic );
	engine.setChargen ( (const uint8_t*)character );
}
//-----------------------------------------------------------------------------

void Player::setSamplerate ( const int _sampleRate )
{
	sampleRate = _sampleRate;
	config.frequency = sampleRate;
}
//-----------------------------------------------------------------------------

bool Player::loadSidFile ( const char* filename )
{
	readyToPlay = false;
	stiEZ = {};

	tune.load ( filename );
	stiEZ.md5 = tune.createMD5New ();

	auto	info = tune.getInfo ();
	if ( ! info )
		return false;

	// Fill basic tune information (global for all songs)
	{
		stiEZ.title = stringutils::extendedASCIItoUTF8 ( info->infoString ( 0 ) );
		stiEZ.author = stringutils::extendedASCIItoUTF8 ( info->infoString ( 1 ) );
		stiEZ.released = stringutils::extendedASCIItoUTF8 ( info->infoString ( 2 ) );

		stiEZ.filename = std::string ( info->path () ) + std::string ( info->dataFileName () );

		stiEZ.numSongs = info->songs ();
		stiEZ.startSong = info->startSong ();

		stiEZ.playroutineID = sidID.findPlayerRoutine ( tune.getSidData () );

		stiEZ.c64LoadAddress = info->loadAddr ();
		stiEZ.c64InitAddress = info->initAddr ();
		stiEZ.c64PlayAddress = info->playAddr ();
		stiEZ.c64DataLength = info->c64dataLen ();
	}

	//
	// Attempt to have better sounding SIDs by adjusting filter-range, digi-boost, and combined waveform strength
	// per author with the assumption they worked with the same machine their entire career
	//
	{
		const auto [ profileName, chipProfile ] = chipSelector.getChipProfile ( info->path (), info->dataFileName () );

		stiEZ.chipProfile = profileName;

		engine.set6581FilterRange ( chipProfile.fltCox );
		engine.set6581FilterCurve ( chipProfile.flt0Dac );
		engine.set6581FilterGain ( chipProfile.fltGain );

		engine.set6581DigiVolume ( chipProfile.digi );

		engine.setCombinedWaveforms ( reSIDfp::CombinedWaveforms ( chipProfile.cwsLevel ), float ( chipProfile.cwsThreshold ) );
	}

	return tune.getStatus ();
}
//-----------------------------------------------------------------------------

bool libsidplayEZ::Player::setTuneNumber ( const unsigned int songNo )
{
	readyToPlay = false;

	// Select song
	stiEZ.currentSong = tune.selectSong ( songNo );

	auto	info = tune.getInfo ();
	if ( ! info )
		return false;

	// Initialize SID engine(s)
	config.playback = info->sidChips () == 1 ? SidConfig::playback_t::MONO : SidConfig::playback_t::STEREO;

	if ( ! engine.setConfig ( config ) )
		return false;

	// Load the tune
	readyToPlay = engine.loadTune ( &tune );

	if ( ! readyToPlay )
		return false;

	// Fill the info struct for this particular tune
	{
		// Model(s)
		for ( auto i = 0; i < engine.getNumChips (); ++i )
			stiEZ.model.emplace_back ( info->sidModel ( i ) == SidTuneInfo::model_t::SIDMODEL_8580 ? "8580" : "6581" );

		// Clock
		stiEZ.clock = info->clockSpeed () == SidTuneInfo::clock_t::CLOCK_NTSC ? "NTSC" : "PAL";

		// Speed
		const auto& engineInfo = (const SidInfoImpl&)engine.getInfo ();

		stiEZ.speed = engineInfo.speedString ();
	}

	// Override chip-profile for Emulation based SID editors (Cheesecutter, GoatTracker, SidWizard etc.)
	{
		if ( stiEZ.model[ 0 ] == "6581" )
		{
			auto oldEmulation = [ this ]
			{
				stiEZ.chipProfile = "Editor uses reSID emulation";

				engine.set6581FilterRange ( 0.5 );
				engine.set6581FilterCurve ( 0.5 );
				engine.set6581FilterGain ( 1.0 );
				engine.set6581DigiVolume ( 1.0 );
				engine.setCombinedWaveforms ( reSIDfp::CombinedWaveforms::STRONG, 1.0 );
			};

			static const std::vector<std::string>	editorsUsingEmulation = {
				"CheeseCutter_1", "GoatTracker_V", "SidWizard_", "Hermit/SidWizard_V",
			};

			for ( const auto& id : editorsUsingEmulation )
				if ( stiEZ.playroutineID.starts_with ( id ) )
					oldEmulation ();
		}
	}

	return readyToPlay;
}
//-----------------------------------------------------------------------------

}
//-----------------------------------------------------------------------------
