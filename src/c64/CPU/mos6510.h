#pragma once
/*
* This file is part of libsidplayfp, a SID player engine.
*
* Copyright 2011-2019 Leandro Nini <drfiemost@users.sourceforge.net>
* Copyright 2007-2010 Antti Lankila
* Copyright 2000 Simon White
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

#include "flags.h"
#include "EventCallback.h"
#include "EventScheduler.h"

class EventContext;

namespace libsidplayfp
{

/**
* Cycle-exact 6502/6510 emulation core
*
* Code is based on work by Simon A. White <sidplay2@yahoo.com>
* Original Java port by Ken Händel. Later on, it has been hacked to improve compatibility with Lorenz suite on VICE's test suite
*
* @author alankila
*/
class MOS6510
{
public:
	class haltInstruction {};

private:
	/**
	* IRQ/NMI magic limit values.
	* Need to be larger than about 0x103 << 3,
	* but can't be min/max for Integer type.
	*/
	static const int MAX = 65536;

	// Stack page location
	static const uint8_t SP_PAGE = 0x01;

public:
	// Status register interrupt bit.
	static const int SR_INTERRUPT = 2;

private:
	struct ProcessorCycle
	{
		void ( MOS6510::* func )() = nullptr;
		bool nosteal = false;
	};

private:
	// Event scheduler
	EventScheduler& eventScheduler;

	// Current instruction and subcycle within instruction
	int cycleCount;

	// When IRQ was triggered. -MAX means "during some previous instruction", MAX means "no IRQ"
	int interruptCycle;

	// IRQ asserted on CPU
	bool irqAssertedOnPin;

	// NMI requested?
	bool nmiFlag;

	// RST requested?
	bool rstFlag;

	// RDY pin state (stop CPU on read)
	bool rdy;

	// Address Low summer carry
	bool adl_carry;

	bool d1x1;

	// The RDY pin state during last throw away read.
	bool rdyOnThrowAwayRead;

	// Status register
	Flags flags;

	// Data regarding current instruction
	uint16_t Register_ProgramCounter;
	uint16_t Cycle_EffectiveAddress;
	uint16_t Cycle_Pointer;

	uint8_t Cycle_Data;
	uint8_t Register_StackPointer;
	uint8_t Register_Accumulator;
	uint8_t Register_X;
	uint8_t Register_Y;

	// Table of CPU opcode implementations
	struct ProcessorCycle instrTable[ 0x101 << 3 ];

private:
	EventCallback<MOS6510> m_nosteal;	// Represents an instruction subcycle that writes
	EventCallback<MOS6510> m_steal;		// Represents an instruction subcycle that reads
	EventCallback<MOS6510> clearInt;

	void eventWithoutSteals ();
	void eventWithSteals ();
	void removeIRQ ();

	void Initialise ();

	// Declare Interrupt Routines
	void IRQLoRequest ();
	void IRQHiRequest ();
	void interruptsAndNextOpcode ();
	void calculateInterruptTriggerCycle ();

	// Declare Instruction Routines
	void fetchNextOpcode ();
	void throwAwayFetch ();
	void throwAwayRead ();
	void FetchDataByte ();
	void FetchLowAddr ();
	void FetchLowAddrX ();
	void FetchLowAddrY ();
	void FetchHighAddr ();
	void FetchHighAddrX ();
	void FetchHighAddrX2 ();
	void FetchHighAddrY ();
	void FetchHighAddrY2 ();
	void FetchLowEffAddr ();
	void FetchHighEffAddr ();
	void FetchHighEffAddrY ();
	void FetchHighEffAddrY2 ();
	void FetchLowPointer ();
	void FetchLowPointerX ();
	void FetchHighPointer ();
	void FetchEffAddrDataByte ();
	void PutEffAddrDataByte ();
	void PushLowPC ();
	void PushHighPC ();
	void PushSR ();
	void PopLowPC ();
	void PopHighPC ();
	void PopSR ();
	void brkPushLowPC ();
	void WasteCycle ();

	void Push ( uint8_t data );
	uint8_t Pop ();
	void compare ( uint8_t data );

	// Declare Instruction Operation Routines
	void adc_instr ();
	void alr_instr ();
	void anc_instr ();
	void and_instr ();
	void ane_instr ();
	void arr_instr ();
	void asl_instr ();
	void asla_instr ();
	void aso_instr ();
	void axa_instr ();
	void axs_instr ();
	void bcc_instr ();
	void bcs_instr ();
	void beq_instr ();
	void bit_instr ();
	void bmi_instr ();
	void bne_instr ();
	void branch_instr ( bool condition );
	void fix_branch ();
	void bpl_instr ();
	void bvc_instr ();
	void bvs_instr ();
	void clc_instr ();
	void cld_instr ();
	void cli_instr ();
	void clv_instr ();
	void cmp_instr ();
	void cpx_instr ();
	void cpy_instr ();
	void dcm_instr ();
	void dec_instr ();
	void dex_instr ();
	void dey_instr ();
	void eor_instr ();
	void inc_instr ();
	void ins_instr ();
	void inx_instr ();
	void iny_instr ();
	void jmp_instr ();
	void las_instr ();
	void lax_instr ();
	void lda_instr ();
	void ldx_instr ();
	void ldy_instr ();
	void lse_instr ();
	void lsr_instr ();
	void lsra_instr ();
	void oal_instr ();
	void ora_instr ();
	void pha_instr ();
	void pla_instr ();
	void rla_instr ();
	void rol_instr ();
	void rola_instr ();
	void ror_instr ();
	void rora_instr ();
	void rra_instr ();
	void rti_instr ();
	void rts_instr ();
	void sbx_instr ();
	void say_instr ();
	void sbc_instr ();
	void sec_instr ();
	void sed_instr ();
	void sei_instr ();
	void shs_instr ();
	void sta_instr ();
	void stx_instr ();
	void sty_instr ();
	void tax_instr ();
	void tay_instr ();
	void tsx_instr ();
	void txa_instr ();
	void txs_instr ();
	void tya_instr ();
	void xas_instr ();
	void sh_instr ();

	/**
	* @throws haltInstruction
	*/
	void invalidOpcode ();

	// Declare Arithmetic Operations
	void doADC ();
	void doSBC ();

	inline bool checkInterrupts () const { return rstFlag || nmiFlag || ( irqAssertedOnPin && ! flags.getI () ); }

	void buildInstructionTable ();

protected:
	MOS6510 ( EventScheduler& scheduler );

	/**
	* Get data from system environment.
	*
	* @param address
	* @return data byte CPU requested
	*/
	virtual uint8_t cpuRead ( uint16_t addr ) = 0;

	/**
	* Write data to system environment.
	*
	* @param address
	* @param data
	*/
	virtual void cpuWrite ( uint16_t addr, uint8_t data ) = 0;

public:
	void reset ();

	static const char* credits ();

	void setRDY ( bool newRDY );

	// Non-standard functions
	void triggerRST ();
	void triggerNMI ();
	void triggerIRQ ();
	void clearIRQ ();
};
//-----------------------------------------------------------------------------

}
