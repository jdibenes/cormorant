//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "gb_pak.h"
#include "gb_rtc.h"

static u8  *g_ROM;
static u8  *g_SRAM;
static bool g_hasRTC;
static bool g_hasrumble;
static u32  g_ROMmask;
static u32  g_SRAMmask;

static void (*g_ROMmap)(u16, u8);
static void (*g_SRAMwrite)(u16, u8);
static u8   (*g_SRAMread)(u16);

static u8  *g_memorymap[0x10];
static s32  g_ROMbank;
static s32  g_ROMbanklo;
static s32  g_ROMbankhi;
static s32  g_MBC1model;
static s32  g_SRAMbank;
static s32  g_RTClatch;
static bool g_SRAMenable;

//-----------------------------------------------------------------------------
// I/O
//-----------------------------------------------------------------------------

void gb_WriteROM(u16 offset, u8 byte)
{
    (*g_ROMmap)(offset, byte);
}

void gb_WriteSRAM(u16 offset, u8 byte)
{
    (*g_SRAMwrite)(offset, byte);
}

u8 gb_ReadROM(u16 offset)
{
    return g_memorymap[offset >> 12][offset & 0x0FFF];
}

u8 gb_ReadSRAM(u16 offset)
{
    return (*g_SRAMread)(offset);
}

//-----------------------------------------------------------------------------
// Bank switching
//-----------------------------------------------------------------------------

static void gb_SwitchROMbank(int bank)
{
    for (s32 i = 0; i < 4; i++) { g_memorymap[0x04 + i] = &g_ROM[bank * 0x4000 + 0x1000 * i]; }
}

static void gb_SwitchSRAMbank(int bank)
{
    for (s32 i = 0; i < 2; i++) { g_memorymap[0x0A + i] = &g_SRAM[bank * 0x2000 + 0x1000 * i]; }
}

//-----------------------------------------------------------------------------
// No MBC
//-----------------------------------------------------------------------------

static void gb_ROMmapMBCnone(u16, u8)
{
    // NOP
}

static void gb_SRAMwriteMBCnone(u16 offset, u8 byte)
{
    g_memorymap[offset >> 12][offset & 0x0FFF] = byte;
}

static u8 gb_SRAMreadMBCnone(u16 offset)
{
    return g_memorymap[offset >> 12][offset & 0x0FFF];
}

//-----------------------------------------------------------------------------
// MBC1
//-----------------------------------------------------------------------------

static void gb_ROMmapMBC1(u16 offset, u8 byte)
{
    switch (offset >> 12)
    {
    case 0x00:
    case 0x01:
        g_SRAMenable = ((byte & 0x0F) == 0x0A);
        break;
    case 0x02:
    case 0x03:
        g_ROMbanklo = byte & 0x1F;
        g_ROMbank = (g_MBC1model ? g_ROMbanklo : (g_ROMbankhi | g_ROMbanklo)) & g_ROMmask;
        if (!(g_ROMbank & 0x1F)) { g_ROMbank++; }
        gb_SwitchROMbank(g_ROMbank);
        break;
    case 0x04:
    case 0x05:
        if (g_MBC1model)
        {
            g_SRAMbank = byte & g_SRAMmask;
            gb_SwitchSRAMbank(g_SRAMbank);
        }
        else
        {
            g_ROMbankhi = (byte & 0x03) << 5;
            g_ROMbank = (g_ROMbankhi | g_ROMbanklo) & g_ROMmask;
            if (!(g_ROMbank & 0x1F)) { g_ROMbank++; }
            gb_SwitchROMbank(g_ROMbank);
        }
        break;
    case 0x06:
    case 0x07:
        g_MBC1model = byte & 0x01;
        break;
    }
}

static void gb_SRAMwriteMBC1(u16 offset, u8 byte)
{
    if (g_SRAMenable) { g_memorymap[offset >> 12][offset & 0x0FFF] = byte; }
}

static u8 gb_SRAMreadMBC1(u16 offset)
{
    return g_memorymap[offset >> 12][offset & 0x0FFF];
}

//-----------------------------------------------------------------------------
// MBC2
//-----------------------------------------------------------------------------

static void gb_ROMmapMBC2(u16 offset, u8 byte)
{
    switch (BIT_FIELD(offset, 8, 1))
    {
    case 0:
        g_SRAMenable = ((byte & 0x0F) == 0x0A);
        break;
    case 1:
        g_ROMbank = byte & g_ROMmask;
        if (!g_ROMbank) { g_ROMbank = 1; }
        gb_SwitchROMbank(g_ROMbank);
        break;
    }
}

static void gb_SRAMwriteMBC2(u16 offset, u8 byte)
{
    if (g_SRAMenable) { g_memorymap[0xA][offset & 0x1FF] = byte & 0x0F; }
}

static u8 gb_SRAMreadMBC2(u16 offset)
{
    return g_memorymap[0xA][offset & 0x1FF];
}

//-----------------------------------------------------------------------------
// MBC3
//-----------------------------------------------------------------------------

static void gb_ROMmapMBC3(u16 offset, u8 byte)
{
    switch (offset >> 12)
    {
    case 0x00:
    case 0x01:
        g_SRAMenable = ((byte & 0x0F) == 0x0A);
        break;
    case 0x02:
    case 0x03:
        g_ROMbank = byte & g_ROMmask;
        if (!g_ROMbank) { g_ROMbank = 1; }
        gb_SwitchROMbank(g_ROMbank);
        break;
    case 0x04:
    case 0x05:
        g_SRAMbank = byte;
        if (g_hasRTC && byte >= 0x08 && byte <= 0x0C) { break; }
        g_SRAMbank = byte & g_SRAMmask;
        gb_SwitchSRAMbank(g_SRAMbank);
        break;
    case 0x06:
    case 0x07:
        if (!g_hasRTC) { break; }
        if (g_RTClatch == 0 && byte == 1) { gb_LatchRTC(); }
        g_RTClatch = byte;
        break;
    }
}

static void gb_SRAMwriteMBC3(u16 offset, u8 byte)
{
    if (!g_SRAMenable) { return; }
    if (g_SRAMbank >= 0x08 && g_SRAMbank <= 0x0C) { gb_WriteRTC(g_SRAMbank, byte); } else { g_memorymap[offset >> 12][offset & 0x0FFF] = byte; }
}

static u8 gb_SRAMreadMBC3(u16 offset)
{
    u8 ret;
    if (g_SRAMbank >= 0x08 && g_SRAMbank <= 0x0C) { ret = gb_ReadRTC(g_SRAMbank); } else { ret = g_memorymap[offset >> 12][offset & 0x0FFF]; }
    return ret;
}

//-----------------------------------------------------------------------------
// MBC5
//-----------------------------------------------------------------------------

static void gb_ROMmapMBC5(u16 offset, u8 byte)
{
    switch (offset >> 12)
    {
    case 0x00:
    case 0x01:
        g_SRAMenable = ((byte & 0x0F) == 0x0A);
        break;
    case 0x02:
        g_ROMbanklo = byte;
        g_ROMbank = (g_ROMbanklo | g_ROMbankhi) & g_ROMmask;
        gb_SwitchROMbank(g_ROMbank);
        break;
    case 0x03:
        g_ROMbankhi = ((u16)byte << 8) & 0x0100;
        g_ROMbank = (g_ROMbanklo | g_ROMbankhi) & g_ROMmask;
        gb_SwitchROMbank(g_ROMbank);
        break;
    case 0x04:
    case 0x05:
        g_SRAMbank = (g_hasrumble ? (byte & 0x07) : (byte & 0x0F)) & g_SRAMmask;
        gb_SwitchSRAMbank(g_SRAMbank);
        break;
    }
}

static void gb_SRAMwriteMBC5(u16 offset, u8 byte)
{
    if (g_SRAMenable) { g_memorymap[offset >> 12][offset & 0x0FFF] = byte; }
}

static u8 gb_SRAMreadMBC5(u16 offset)
{
    return g_memorymap[offset >> 12][offset & 0x0FFF];
}

//-----------------------------------------------------------------------------
// Control
//-----------------------------------------------------------------------------

void gb_ResetMBC()
{
    g_ROM       =  gb_GetPakROMAddress();
    g_SRAM      =  gb_GetPakSRAMAddress();
    g_hasRTC    =  gb_GetPakRTC();
    g_hasrumble =  gb_GetPakRumble();
    g_ROMmask   = (gb_GetPakROMSize()  / (16 * 1024)) - 1;
    g_SRAMmask  =  gb_GetPakSRAMSize() / ( 8 * 1024);
    if (g_SRAMmask > 0) { g_SRAMmask--; }

    switch (gb_GetPakMBC())
    {
    case MBC_1: g_ROMmap = &gb_ROMmapMBC1;    g_SRAMwrite = &gb_SRAMwriteMBC1;    g_SRAMread = &gb_SRAMreadMBC1;    break;
    case MBC_2: g_ROMmap = &gb_ROMmapMBC2;    g_SRAMwrite = &gb_SRAMwriteMBC2;    g_SRAMread = &gb_SRAMreadMBC2;    break;
    case MBC_3: g_ROMmap = &gb_ROMmapMBC3;    g_SRAMwrite = &gb_SRAMwriteMBC3;    g_SRAMread = &gb_SRAMreadMBC3;    break;
    case MBC_5: g_ROMmap = &gb_ROMmapMBC5;    g_SRAMwrite = &gb_SRAMwriteMBC5;    g_SRAMread = &gb_SRAMreadMBC5;    break;
    default:    g_ROMmap = &gb_ROMmapMBCnone; g_SRAMwrite = &gb_SRAMwriteMBCnone; g_SRAMread = &gb_SRAMreadMBCnone; break;
    }

    for (s32 i = 0; i < 8; i++) { g_memorymap[i]        = &g_ROM[ 0x1000 * i]; }
    for (s32 i = 0; i < 2; i++) { g_memorymap[0x0A + i] = &g_SRAM[0x1000 * i]; }
    
    g_ROMbank    = 1;
    g_ROMbanklo  = 0;
    g_ROMbankhi  = 0;
    g_MBC1model  = 0;
    g_SRAMbank   = 0;
    g_RTClatch   = 1;
    g_SRAMenable = false;
}
