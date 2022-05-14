//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

void gb_ResetSerial();
void gb_WriteSB(u8 byte);
void gb_WriteSC(u8 byte);
u8   gb_ReadSB();
u8   gb_ReadSC();
void gb_SyncSerial(s32 ticks);
