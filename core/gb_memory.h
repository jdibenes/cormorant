//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

void gb_ResetMemory();
void gb_WriteByte(u16 offset, u8 byte);
void gb_WriteIO(u8 port, u8 byte);
u8   gb_ReadByte(u16 offset);
u8   gb_ReadIO(u8 port);
