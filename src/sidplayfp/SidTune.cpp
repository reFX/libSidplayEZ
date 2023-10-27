/*
 * This file is part of libsidplayfp, a SID player engine.
 *
 * Copyright 2012-2021 Leandro Nini <drfiemost@users.sourceforge.net>
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

#include "SidTune.h"
#include "sidtune/SidTuneBase.h"

using namespace libsidplayfp;

const char MSG_NO_ERRORS[] = "No errors";

//-----------------------------------------------------------------------------

SidTune::SidTune ( const char* fileName, bool separatorIsSlash )
	: SidTune ( nullptr, fileName, separatorIsSlash )
{
}
//-----------------------------------------------------------------------------

SidTune::SidTune ( LoaderFunc loader, const char* fileName, bool separatorIsSlash )
	: tune ( nullptr )
{
	load ( loader, fileName, separatorIsSlash );
}
//-----------------------------------------------------------------------------

SidTune::SidTune ( const uint8_t* oneFileFormatSidtune, uint32_t sidtuneLength )
	: tune ( nullptr )
{
	read ( oneFileFormatSidtune, sidtuneLength );
}
//-----------------------------------------------------------------------------

SidTune::~SidTune ()
{
	delete tune;
}
//-----------------------------------------------------------------------------

void SidTune::load ( const char* fileName, bool separatorIsSlash )
{
	load ( nullptr, fileName, separatorIsSlash );
}
//-----------------------------------------------------------------------------

void SidTune::load ( LoaderFunc loader, const char* fileName, bool separatorIsSlash )
{
	try
	{
		delete tune;
		tune = SidTuneBase::load ( loader, fileName, separatorIsSlash );
		m_status = true;
		m_statusString = MSG_NO_ERRORS;
	}
	catch ( loadError const& e )
	{
		tune = nullptr;
		m_status = false;
		m_statusString = e.message ();
	}
}
//-----------------------------------------------------------------------------

void SidTune::read ( const uint8_t* sourceBuffer, uint32_t bufferLen )
{
	try
	{
		delete tune;
		tune = SidTuneBase::read ( sourceBuffer, bufferLen );
		m_status = true;
		m_statusString = MSG_NO_ERRORS;
	}
	catch ( loadError const& e )
	{
		tune = nullptr;
		m_status = false;
		m_statusString = e.message ();
	}
}
//-----------------------------------------------------------------------------

unsigned int SidTune::selectSong ( unsigned int songNum )
{
	return tune ? tune->selectSong ( songNum ) : 0;
}
//-----------------------------------------------------------------------------

const SidTuneInfo* SidTune::getInfo () const
{
	return tune ? tune->getInfo () : nullptr;
}
//-----------------------------------------------------------------------------

const SidTuneInfo* SidTune::getInfo ( unsigned int songNum )
{
	return tune ? tune->getInfo ( songNum ) : nullptr;
}
//-----------------------------------------------------------------------------

bool SidTune::getStatus () const { return m_status; }

const char* SidTune::statusString () const { return m_statusString; }

//-----------------------------------------------------------------------------

bool SidTune::placeSidTuneInC64mem ( sidmemory& mem )
{
	if ( ! tune )
		return false;

	tune->placeSidTuneInC64mem ( mem );
	return true;
}
//-----------------------------------------------------------------------------

const char* SidTune::createMD5 ( char* md5 )
{
	return tune ? tune->createMD5 ( md5 ) : nullptr;
}
//-----------------------------------------------------------------------------

const char* SidTune::createMD5New ( char* md5 )
{
	return tune ? tune->createMD5New ( md5 ) : nullptr;
}
//-----------------------------------------------------------------------------

const uint8_t* SidTune::c64Data () const
{
	return tune ? tune->c64Data () : nullptr;
}
//-----------------------------------------------------------------------------
