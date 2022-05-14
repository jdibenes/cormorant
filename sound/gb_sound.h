//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

void gb_ResetSound();

void gb_WriteNR10(u8 byte);
void gb_WriteNR11(u8 byte);
void gb_WriteNR12(u8 byte);
void gb_WriteNR13(u8 byte);
void gb_WriteNR14(u8 byte);
void gb_WriteNR21(u8 byte);
void gb_WriteNR22(u8 byte);
void gb_WriteNR23(u8 byte);
void gb_WriteNR24(u8 byte);
void gb_WriteNR30(u8 byte);
void gb_WriteNR31(u8 byte);
void gb_WriteNR32(u8 byte);
void gb_WriteNR33(u8 byte);
void gb_WriteNR34(u8 byte);
void gb_WriteNR41(u8 byte);
void gb_WriteNR42(u8 byte);
void gb_WriteNR43(u8 byte);
void gb_WriteNR44(u8 byte);
void gb_WriteNR50(u8 byte);
void gb_WriteNR51(u8 byte);
void gb_WriteNR52(u8 byte);
void gb_WriteWaveRAM(u8 port, u8 byte);

u8 gb_ReadNR10();
u8 gb_ReadNR11();
u8 gb_ReadNR12();
u8 gb_ReadNR13();
u8 gb_ReadNR14();
u8 gb_ReadNR21();
u8 gb_ReadNR22();
u8 gb_ReadNR23();
u8 gb_ReadNR24();
u8 gb_ReadNR30();
u8 gb_ReadNR31();
u8 gb_ReadNR32();
u8 gb_ReadNR33();
u8 gb_ReadNR34();
u8 gb_ReadNR41();
u8 gb_ReadNR42();
u8 gb_ReadNR43();
u8 gb_ReadNR44();
u8 gb_ReadNR50();
u8 gb_ReadNR51();
u8 gb_ReadNR52();
u8 gb_ReadWaveRAM(u8 port);

void gb_SyncSound(s32 ticks);
