//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "types.h"

void gb_Initialize();
void gb_SetSoundPeriod(s32 clk);
int  gb_StartEmulation();
void gb_StopEmulation();
