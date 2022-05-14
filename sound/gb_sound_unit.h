//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

void gb_ResetSoundUnit();
void gb_SyncSoundUnit(s32 ticks);
s32  gb_GetSampleSOX(s32 terminal);
