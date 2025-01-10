#pragma once

#include "../player.h"
#include "sidid.h"
#include "chip-selector.h"
#include "SidTuneInfoEZ.h"

namespace libsidplayEZ
{

//-----------------------------------------------------------------------------

class Player final
{
public:
	Player ();
	~Player () = default;

	bool loadSidIDConfig ( const char* filename ) { return sidID.loadSidIDConfig ( filename ); }
	void setChipProfileMap ( const ChipSelector::profileMap& map ) { chipSelector.setProfiles ( map ); }

	void setSamplerate ( const int _sampleRate );
	bool isReadyToPlay () const { return readyToPlay; }
	bool loadSidFile ( const char* filename );
	bool init ( const unsigned int songNo = 0 );

	[[ nodiscard ]] int getNumChips () const { return engine.getNumChips (); }
	[[ nodiscard ]] const int getNumOutChannels () const { return config.playback; }

	[[ nodiscard ]] const SidTuneInfoEZ& getFileInfo () const	{	return stiEZ;	}
	[[ nodiscard ]] const SidTuneInfo* getSidTuneInfo () const	{	return tune.getInfo ();	}
	[[ nodiscard ]] const SidTune& getSidTune () const { return tune; }

	[[ nodiscard ]] unsigned int getCurrentSong () const;
	[[ nodiscard ]] std::string getChipProfile () const { return selectedChipProfile; }

	libsidplayfp::Player	engine;

private:
	bool	readyToPlay = false;
	int		sampleRate = 0;

	const char* md5 = nullptr;

	ChipSelector	chipSelector;
	std::string		selectedChipProfile;

	SidTune		tune;
	SidConfig	config;

	sidid		sidID;

	SidTuneInfoEZ	stiEZ;
};
//-----------------------------------------------------------------------------

}
