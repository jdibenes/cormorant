//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

void gb_ResetSpeed();
void gb_WriteKEY1(u8 byte);
u8   gb_ReadKEY1();
bool gb_CGBSpeed();
void gb_CGBSetSpeed();
