//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

void gb_ResetMBC();
void gb_WriteROM(u16 offset, u8 byte);
void gb_WriteSRAM(u16 offset, u8 byte);
u8   gb_ReadROM(u16 offset);
u8   gb_ReadSRAM(u16 offset);
