//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

enum gb_MBC
{
    MBC_NONE,
    MBC_1,
    MBC_2,
    MBC_3,
    MBC_5,
    MBC_UNKNOWN
};

struct gb_ROMHeader
{
    u8 logo[48];
    u8 title[15];
    u8 CGBFlag;
    u8 newcode[2];
    u8 SGBflag;
    u8 paktype;
    u8 ROMsize;
    u8 SRAMsize;
    u8 destinationcode;
    u8 oldcode;
    u8 ROMversion;
    u8 headerchecksum;
    u8 globalchecksum[2];
};

void   gb_SetPakROMAddress(u8 *ROM);
void   gb_SetPakSRAMAddress(u8 *SRAM);
u8    *gb_GetPakROMAddress();
u8    *gb_GetPakSRAMAddress();
u8     gb_ComputeHeaderChecksum(gb_ROMHeader const *header);
void   gb_SetPakFeatures(gb_ROMHeader const *header);
u32    gb_ValidatePak();
u32    gb_GetPakROMSize();
u32    gb_GetPakSRAMSize();
gb_MBC gb_GetPakMBC();
bool   gb_GetPakSRAM();
bool   gb_GetPakRTC();
bool   gb_GetPakBattery();
bool   gb_GetPakRumble();
bool   gb_IsCGB();
