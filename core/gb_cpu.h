//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

void gb_ResetCPU();
void gb_CheckInterrupts();
bool gb_isHalted();
s32  gb_ExecuteNextInstruction();
void gb_UnknownOpcodeHandler(bool CB, u8 opcode, u16 PC, u16 AF, u16 BC, u16 DE, u16 HL, u16 SP, bool IME);
void gb_BuildInterpreter();

