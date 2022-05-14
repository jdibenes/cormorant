//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

void gb_ResetLCD();

void gb_WriteVRAM(u16 offset, u8 byte);
void gb_WriteOAM(u16 offset, u8 byte);
u8   gb_ReadVRAM(u16 offset);
u8   gb_ReadOAM(u16 offset);

void gb_WriteLCDC(u8 byte);
void gb_WriteSTAT(u8 byte);
void gb_WriteSCY(u8 byte);
void gb_WriteSCX(u8 byte);
void gb_WriteLYC(u8 byte);
void gb_WriteDMA(u8 byte);
void gb_WriteBGP(u8 byte);
void gb_WriteOBP0(u8 byte);
void gb_WriteOBP1(u8 byte);
void gb_WriteWY(u8 byte);
void gb_WriteWX(u8 byte);
void gb_WriteVBK(u8 byte);
void gb_WriteHDMA1(u8 byte);
void gb_WriteHDMA2(u8 byte);
void gb_WriteHDMA3(u8 byte);
void gb_WriteHDMA4(u8 byte);
void gb_WriteHDMA5(u8 byte);
void gb_WriteBCPS(u8 byte);
void gb_WriteBCPD(u8 byte);
void gb_WriteOCPS(u8 byte);
void gb_WriteOCPD(u8 byte);

u8   gb_ReadLCDC();
u8   gb_ReadSTAT();
u8   gb_ReadSCY();
u8   gb_ReadSCX();
u8   gb_ReadLY();
u8   gb_ReadLYC();
u8   gb_ReadDMA();
u8   gb_ReadBGP();
u8   gb_ReadOBP0();
u8   gb_ReadOBP1();
u8   gb_ReadWY();
u8   gb_ReadWX();
u8   gb_ReadVBK();
u8   gb_ReadHDMA1();
u8   gb_ReadHDMA2();
u8   gb_ReadHDMA3();
u8   gb_ReadHDMA4();
u8   gb_ReadHDMA5();
u8   gb_ReadBCPS();
u8   gb_ReadBCPD();
u8   gb_ReadOCPS();
u8   gb_ReadOCPD();

s32  gb_SyncHDMA();
void gb_SyncLCD(s32 ticks);
