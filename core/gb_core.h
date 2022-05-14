//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

#define IRQ_VBL 0x01
#define IRQ_LCD 0x02
#define IRQ_TMR 0x04
#define IRQ_LNK 0x08
#define IRQ_PAD 0x10

void gb_RequestInterrupt(u8 IRQ);
