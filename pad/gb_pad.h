//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

void gb_ResetJoypad();
void gb_WriteP1(u8 byte);
u8   gb_ReadP1();
void gb_SyncJoypad();
