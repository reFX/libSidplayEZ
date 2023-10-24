#pragma once
/*
 * This file is part of libsidplayfp, a SID player engine.
 *
 * Copyright 2011-2019 Leandro Nini <drfiemost@users.sourceforge.net>
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

#include <string>

#include "sidplayfp/SidConfig.h"
#include "Event.h"
#include "EventScheduler.h"

#include "c64/c64sid.h"

class sidbuilder;

namespace libsidplayfp
{

/**
 * Inherit this class to create a new SID emulation.
 */
class sidemu : public c64sid
{
public:
    /**
     * Buffer size. 5000 is roughly 5 ms at 96 kHz
     */
    enum
    {
        OUTPUTBUFFERSIZE = 5000
    };

private:
    sidbuilder* const m_builder;

protected:
    static const char ERR_UNSUPPORTED_FREQ[];
    static const char ERR_INVALID_SAMPLING[];
    static const char ERR_INVALID_CHIP[];

protected:
    EventScheduler *eventScheduler;

    event_clock_t m_accessClk;

    /// The sample buffer
    short *m_buffer;

    /// Current position in buffer
    int m_bufferpos;

    bool m_status;
    bool isLocked;

    std::string m_error;

public:
    sidemu(sidbuilder *builder) :
        m_builder(builder),
        eventScheduler(nullptr),
        m_buffer(nullptr),
        m_bufferpos(0),
        m_status(true),
        isLocked(false),
        m_error("N/A") {}
    virtual ~sidemu() {}

    /**
     * Clock the SID chip.
     */
    virtual void clock() = 0;

    /**
     * Set execution environment and lock sid to it.
     */
    virtual bool lock(EventScheduler *scheduler);

    /**
     * Unlock sid.
     */
    virtual void unlock();

    // Standard SID functions
    
    /**
     * Set SID model.
     */
    virtual void model(SidConfig::sid_model_t model, bool digiboost) = 0;

    /**
     * Set the sampling method.
     *
     * @param systemfreq
     * @param outputfreq
     * @param method
     * @param fast
     */
    virtual void sampling( [[ maybe_unused ]]float systemfreq, [[ maybe_unused ]] float outputfreq ) {}

    /**
     * Get a detailed error message.
     */
    const char* error() const { return m_error.c_str(); }

    sidbuilder* builder() const { return m_builder; }

    /**
     * Get the current position in buffer.
     */
    int bufferpos() const { return m_bufferpos; }

    /**
     * Set the position in buffer.
     */
    void bufferpos(int pos) { m_bufferpos = pos; }

    /**
     * Get the buffer.
     */
    short *buffer() const { return m_buffer; }
};

}
