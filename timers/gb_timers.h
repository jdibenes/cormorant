//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

void gb_ResetTimers();
void gb_WriteDIV(u8 byte);
void gb_WriteTIMA(u8 byte);
void gb_WriteTMA(u8 byte);
void gb_WriteTAC(u8 byte);
u8   gb_ReadDIV();
u8   gb_ReadTIMA();
u8   gb_ReadTMA();
u8   gb_ReadTAC();
void gb_SyncTimers(s32 ticks);
