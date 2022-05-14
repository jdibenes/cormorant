//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

void gb_ResetIRQ();
void gb_WriteIF(u8 byte);
void gb_WriteIE(u8 byte);
u8   gb_ReadIF();
u8   gb_ReadIE();
u8   gb_IRQsPending();
void gb_AcknowledgeInterrupt(u8 IRQ);
