//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

void gb_LatchRTC();
void gb_ResetRTC();
void gb_WriteRTC(u8 port, u8 byte);
u8   gb_ReadRTC(u8 port);
