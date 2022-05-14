//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "gb_pak.h"

struct gb_PakFeatures
{
    u32    ROMsize;
    u32    SRAMsize;
    gb_MBC MBC;
    bool   hasSRAM;
    bool   hasbattery;
    bool   hasRTC;
    bool   hasrumble;
    bool   iscgb;
};

static gb_PakFeatures g_pak;

static u8 *g_ROM  = NULL;
static u8 *g_SRAM = NULL;

void gb_SetPakROMAddress(u8 *ROM)
{
    g_ROM = ROM;
}

void gb_SetPakSRAMAddress(u8 *SRAM)
{
    g_SRAM = SRAM;
}

u8 *gb_GetPakROMAddress()
{
    return g_ROM;
}

u8 *gb_GetPakSRAMAddress()
{
    return g_SRAM;
}

u8 gb_ComputeHeaderChecksum(gb_ROMHeader const *header)
{
    u8 const* header_bytes = (u8 *)header;

    u32 title_offset           = 0x30;
    u32 header_checksum_offset = title_offset + 0x19;

    s32 sum = 0;
    for (u32 i = title_offset; i < header_checksum_offset; ++i) { sum -= header_bytes[i] + 1; }
    return sum & 0xFF;
}

void gb_SetPakFeatures(gb_ROMHeader const *header)
{
    switch (header->ROMsize)
    {
    case 0: g_pak.ROMsize =   32 * 1024; break;
    case 1: g_pak.ROMsize =   64 * 1024; break;
    case 2: g_pak.ROMsize =  128 * 1024; break;
    case 3: g_pak.ROMsize =  256 * 1024; break;
    case 4: g_pak.ROMsize =  512 * 1024; break;
    case 5: g_pak.ROMsize = 1024 * 1024; break;
    case 6: g_pak.ROMsize = 2048 * 1024; break;
    case 7: g_pak.ROMsize = 4096 * 1024; break;
    case 8: g_pak.ROMsize = 8192 * 1024; break;
    // case 52:
    // case 53:
    // case 54:
    default: g_pak.ROMsize = 0xFFFFFFFF;
    }

    switch (header->SRAMsize)
    {
    case 0:  g_pak.SRAMsize =   0;        break;
    case 1:  g_pak.SRAMsize =   2 * 1024; break;
    case 2:  g_pak.SRAMsize =   8 * 1024; break;
    case 3:  g_pak.SRAMsize =  32 * 1024; break;
    case 4:  g_pak.SRAMsize = 128 * 1024; break;
    case 5:  g_pak.SRAMsize =  64 * 1024; break;
    default: g_pak.SRAMsize = 0xFFFFFFFF;
    }

    switch (header->paktype)
    {
    case 0x00: g_pak.MBC = MBC_NONE;    g_pak.hasSRAM = false; g_pak.hasRTC = false; g_pak.hasbattery = false; g_pak.hasrumble = false; break; // "ROM"
    case 0x01: g_pak.MBC = MBC_1;       g_pak.hasSRAM = false; g_pak.hasRTC = false; g_pak.hasbattery = false; g_pak.hasrumble = false; break; // "MBC1"
    case 0x02: g_pak.MBC = MBC_1;       g_pak.hasSRAM = true;  g_pak.hasRTC = false; g_pak.hasbattery = false; g_pak.hasrumble = false; break; // "MBC1+SRAM"
    case 0x03: g_pak.MBC = MBC_1;       g_pak.hasSRAM = true;  g_pak.hasRTC = false; g_pak.hasbattery = true;  g_pak.hasrumble = false; break; // "MBC1+SRAM+BATTERY"
    case 0x05: g_pak.MBC = MBC_2;       g_pak.hasSRAM = true;  g_pak.hasRTC = false; g_pak.hasbattery = false; g_pak.hasrumble = false; break; // "MBC2"
    case 0x06: g_pak.MBC = MBC_2;       g_pak.hasSRAM = true;  g_pak.hasRTC = false; g_pak.hasbattery = true;  g_pak.hasrumble = false; break; // "MBC2+BATTERY"
    case 0x08: g_pak.MBC = MBC_NONE;    g_pak.hasSRAM = true;  g_pak.hasRTC = false; g_pak.hasbattery = false; g_pak.hasrumble = false; break; // "ROM+SRAM"
    case 0x09: g_pak.MBC = MBC_NONE;    g_pak.hasSRAM = true;  g_pak.hasRTC = false; g_pak.hasbattery = true;  g_pak.hasrumble = false; break; // "ROM+SRAM+BATTERY"
    // case 0x0B: // MMM01
    // case 0x0C: // MMM01+SRAM
    // case 0x0D: // MMM01+SRAM+BATTERY
    case 0x0F: g_pak.MBC = MBC_3;       g_pak.hasSRAM = false; g_pak.hasRTC = true;  g_pak.hasbattery = true;  g_pak.hasrumble = false; break; // "MBC3+RTC+BATTERY"
    case 0x10: g_pak.MBC = MBC_3;       g_pak.hasSRAM = true;  g_pak.hasRTC = true;  g_pak.hasbattery = true;  g_pak.hasrumble = false; break; // "MBC3+RTC+SRAM+BATTERY"
    case 0x11: g_pak.MBC = MBC_3;       g_pak.hasSRAM = false; g_pak.hasRTC = false; g_pak.hasbattery = false; g_pak.hasrumble = false; break; // "MBC3"
    case 0x12: g_pak.MBC = MBC_3;       g_pak.hasSRAM = true;  g_pak.hasRTC = false; g_pak.hasbattery = false; g_pak.hasrumble = false; break; // "MBC3+SRAM"
    case 0x13: g_pak.MBC = MBC_3;       g_pak.hasSRAM = true;  g_pak.hasRTC = false; g_pak.hasbattery = true;  g_pak.hasrumble = false; break; // "MBC3+SRAM+BATTERY"
    case 0x19: g_pak.MBC = MBC_5;       g_pak.hasSRAM = false; g_pak.hasRTC = false; g_pak.hasbattery = false; g_pak.hasrumble = false; break; // "MBC5"
    case 0x1A: g_pak.MBC = MBC_5;       g_pak.hasSRAM = true;  g_pak.hasRTC = false; g_pak.hasbattery = false; g_pak.hasrumble = false; break; // "MBC5+SRAM"
    case 0x1B: g_pak.MBC = MBC_5;       g_pak.hasSRAM = true;  g_pak.hasRTC = false; g_pak.hasbattery = true;  g_pak.hasrumble = false; break; // "MBC5+SRAM+BATTERY"
    case 0x1C: g_pak.MBC = MBC_5;       g_pak.hasSRAM = false; g_pak.hasRTC = false; g_pak.hasbattery = false; g_pak.hasrumble = true;  break; // "MBC5+RUMBLE"
    case 0x1D: g_pak.MBC = MBC_5;       g_pak.hasSRAM = true;  g_pak.hasRTC = false; g_pak.hasbattery = false; g_pak.hasrumble = true;  break; // "MBC5+RUMBLE+SRAM"
    case 0x1E: g_pak.MBC = MBC_5;       g_pak.hasSRAM = true;  g_pak.hasRTC = false; g_pak.hasbattery = true;  g_pak.hasrumble = true;  break; // "MBC5+RUMBLE+SRAM+BATTERY"
    // case 0x20: // MBC6
    // case 0x22: // MBC7+SENSOR+RUMBLE+SRAM+BATTERY
    // case 0xFC:
    // case 0xFD:
    // case 0xFE:
    // case 0xFF:
    default:   g_pak.MBC = MBC_UNKNOWN; g_pak.hasSRAM = false; g_pak.hasRTC = false; g_pak.hasbattery = false; g_pak.hasrumble = false;
    }

    g_pak.iscgb = BIT_TEST(header->CGBFlag, 7);
}

u32 gb_ValidatePak()
{
    u32 ret = 0;
    if (g_pak.MBC      == MBC_UNKNOWN) { ret |= 1; }
    if (g_pak.ROMsize  == 0xFFFFFFFF)  { ret |= 2; }
    if (g_pak.SRAMsize == 0xFFFFFFFF)  { ret |= 4; }
    return ret;
}

u32 gb_GetPakROMSize()
{
    return g_pak.ROMsize;
}

u32 gb_GetPakSRAMSize()
{
    return (g_pak.MBC == MBC_2) ? 512 : g_pak.SRAMsize;
}

gb_MBC gb_GetPakMBC()
{
    return g_pak.MBC;
}

bool gb_GetPakSRAM()
{
    return g_pak.hasSRAM;
}

bool gb_GetPakRTC()
{
    return g_pak.hasRTC;
}

bool gb_GetPakBattery()
{
    return g_pak.hasbattery;
}

bool gb_GetPakRumble()
{
    return g_pak.hasrumble;
}

bool gb_IsCGB()
{
    return g_pak.iscgb;
}
