//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include <memory.h>
#include "../cartridge/gb_pak.h"
#include "../cartridge/gb_mbc.h"
#include "../cartridge/gb_rtc.h"
#include "../comms/gb_serial.h"
#include "../comms/gb_ir.h"
#include "../display/gb_display.h"
#include "../pad/gb_pad.h"
#include "../sound/gb_sound.h"
#include "../timers/gb_timers.h"
#include "gb_speed.h"
#include "gb_irq.h"

static u8  g_IO[0x100];
static u8  g_WRAM_all[8 * 0x1000];
static u8 *g_WRAM_lo;
static u8 *g_WRAM_hi;
static u8  g_SVBK;

void gb_ResetMemory()
{
    gb_ResetMBC();
    gb_ResetRTC();

    memset(g_WRAM_all, 0, sizeof(g_WRAM_all));
    memset(g_IO,       0, sizeof(g_IO));

    g_SVBK    = 0xF9;
    g_WRAM_lo = &g_WRAM_all[0];
    g_WRAM_hi = &g_WRAM_all[0x1000];
}

static void gb_WriteSVBK(u8 byte)
{
    if (!gb_IsCGB()) { return; }
    u8 bank = BIT_FIELD(byte, 0, 7);
    g_SVBK = byte;
    g_WRAM_hi = &g_WRAM_all[(bank ? bank : 1) * 0x1000];
}

static u8 gb_ReadSVBK()
{
    return gb_IsCGB() ? (g_SVBK | 0xF8) : 0xFF;
}

void gb_WriteIO(u8 port, u8 byte)
{
    switch (port)
    {
    case 0x00: gb_WriteP1(byte);    break;
    case 0x01: gb_WriteSB(byte);    break;
    case 0x02: gb_WriteSC(byte);    break;
    case 0x04: gb_WriteDIV(byte);   break;
    case 0x05: gb_WriteTIMA(byte);  break;
    case 0x06: gb_WriteTMA(byte);   break;
    case 0x07: gb_WriteTAC(byte);   break;
    case 0x0F: gb_WriteIF(byte);    break;
    case 0x10: gb_WriteNR10(byte);  break;
    case 0x11: gb_WriteNR11(byte);  break;
    case 0x12: gb_WriteNR12(byte);  break;
    case 0x13: gb_WriteNR13(byte);  break;
    case 0x14: gb_WriteNR14(byte);  break;
    case 0x16: gb_WriteNR21(byte);  break;
    case 0x17: gb_WriteNR22(byte);  break;
    case 0x18: gb_WriteNR23(byte);  break;
    case 0x19: gb_WriteNR24(byte);  break;
    case 0x1A: gb_WriteNR30(byte);  break;
    case 0x1B: gb_WriteNR31(byte);  break;
    case 0x1C: gb_WriteNR32(byte);  break;
    case 0x1D: gb_WriteNR33(byte);  break;
    case 0x1E: gb_WriteNR34(byte);  break;
    case 0x20: gb_WriteNR41(byte);  break;
    case 0x21: gb_WriteNR42(byte);  break;
    case 0x22: gb_WriteNR43(byte);  break;
    case 0x23: gb_WriteNR44(byte);  break;
    case 0x24: gb_WriteNR50(byte);  break;
    case 0x25: gb_WriteNR51(byte);  break;
    case 0x26: gb_WriteNR52(byte);  break;
    case 0x30: gb_WriteWaveRAM(port, byte); break; // 0
    case 0x31: gb_WriteWaveRAM(port, byte); break; // 1
    case 0x32: gb_WriteWaveRAM(port, byte); break; // 2
    case 0x33: gb_WriteWaveRAM(port, byte); break; // 3
    case 0x34: gb_WriteWaveRAM(port, byte); break; // 4
    case 0x35: gb_WriteWaveRAM(port, byte); break; // 5
    case 0x36: gb_WriteWaveRAM(port, byte); break; // 6
    case 0x37: gb_WriteWaveRAM(port, byte); break; // 7
    case 0x38: gb_WriteWaveRAM(port, byte); break; // 8
    case 0x39: gb_WriteWaveRAM(port, byte); break; // 9
    case 0x3A: gb_WriteWaveRAM(port, byte); break; // 10
    case 0x3B: gb_WriteWaveRAM(port, byte); break; // 11
    case 0x3C: gb_WriteWaveRAM(port, byte); break; // 12
    case 0x3D: gb_WriteWaveRAM(port, byte); break; // 13
    case 0x3E: gb_WriteWaveRAM(port, byte); break; // 14
    case 0x3F: gb_WriteWaveRAM(port, byte); break; // 15
    case 0x40: gb_WriteLCDC(byte);  break;
    case 0x41: gb_WriteSTAT(byte);  break;
    case 0x42: gb_WriteSCY(byte);   break;
    case 0x43: gb_WriteSCX(byte);   break;
    case 0x45: gb_WriteLYC(byte);   break;
    case 0x46: gb_WriteDMA(byte);   break;
    case 0x47: gb_WriteBGP(byte);   break;
    case 0x48: gb_WriteOBP0(byte);  break;
    case 0x49: gb_WriteOBP1(byte);  break;
    case 0x4A: gb_WriteWY(byte);    break;
    case 0x4B: gb_WriteWX(byte);    break;
    case 0x4D: gb_WriteKEY1(byte);  break;
    case 0x4F: gb_WriteVBK(byte);   break;
    case 0x51: gb_WriteHDMA1(byte); break;
    case 0x52: gb_WriteHDMA2(byte); break;
    case 0x53: gb_WriteHDMA3(byte); break;
    case 0x54: gb_WriteHDMA4(byte); break;
    case 0x55: gb_WriteHDMA5(byte); break;
    case 0x56: gb_WriteRP(byte);    break;
    case 0x68: gb_WriteBCPS(byte);  break;
    case 0x69: gb_WriteBCPD(byte);  break;
    case 0x6A: gb_WriteOCPS(byte);  break;
    case 0x6B: gb_WriteOCPD(byte);  break;
    case 0x70: gb_WriteSVBK(byte);  break;
    case 0xFF: gb_WriteIE(byte);    break;
    }

    g_IO[port] = byte; // 0x80-0xFE: HRAM
}

u8 gb_ReadIO(u8 port)
{
    u8 ret;

    switch (port)
    {
    case 0x00: ret = gb_ReadP1();    break;
    case 0x01: ret = gb_ReadSB();    break;
    case 0x02: ret = gb_ReadSC();    break;
    case 0x04: ret = gb_ReadDIV();   break;
    case 0x05: ret = gb_ReadTIMA();  break;
    case 0x06: ret = gb_ReadTMA();   break;
    case 0x07: ret = gb_ReadTAC();   break;
    case 0x0F: ret = gb_ReadIF();    break;    
    case 0x10: ret = gb_ReadNR10();  break;
    case 0x11: ret = gb_ReadNR11();  break;
    case 0x12: ret = gb_ReadNR12();  break;
    case 0x13: ret = gb_ReadNR13();  break;
    case 0x14: ret = gb_ReadNR14();  break;
    case 0x16: ret = gb_ReadNR21();  break;
    case 0x17: ret = gb_ReadNR22();  break;
    case 0x18: ret = gb_ReadNR23();  break;
    case 0x19: ret = gb_ReadNR24();  break;
    case 0x1A: ret = gb_ReadNR30();  break;
    case 0x1B: ret = gb_ReadNR31();  break;
    case 0x1C: ret = gb_ReadNR32();  break;
    case 0x1D: ret = gb_ReadNR33();  break;
    case 0x1E: ret = gb_ReadNR34();  break;
    case 0x20: ret = gb_ReadNR41();  break;
    case 0x21: ret = gb_ReadNR42();  break;
    case 0x22: ret = gb_ReadNR43();  break;
    case 0x23: ret = gb_ReadNR44();  break;
    case 0x24: ret = gb_ReadNR50();  break;
    case 0x25: ret = gb_ReadNR51();  break;
    case 0x26: ret = gb_ReadNR52();  break;
    case 0x30: ret = gb_ReadWaveRAM(port); break; // 0
    case 0x31: ret = gb_ReadWaveRAM(port); break; // 1
    case 0x32: ret = gb_ReadWaveRAM(port); break; // 2
    case 0x33: ret = gb_ReadWaveRAM(port); break; // 3
    case 0x34: ret = gb_ReadWaveRAM(port); break; // 4
    case 0x35: ret = gb_ReadWaveRAM(port); break; // 5
    case 0x36: ret = gb_ReadWaveRAM(port); break; // 6
    case 0x37: ret = gb_ReadWaveRAM(port); break; // 7
    case 0x38: ret = gb_ReadWaveRAM(port); break; // 8
    case 0x39: ret = gb_ReadWaveRAM(port); break; // 9
    case 0x3A: ret = gb_ReadWaveRAM(port); break; // 10
    case 0x3B: ret = gb_ReadWaveRAM(port); break; // 11
    case 0x3C: ret = gb_ReadWaveRAM(port); break; // 12
    case 0x3D: ret = gb_ReadWaveRAM(port); break; // 13
    case 0x3E: ret = gb_ReadWaveRAM(port); break; // 14
    case 0x3F: ret = gb_ReadWaveRAM(port); break; // 15
    case 0x40: ret = gb_ReadLCDC();  break;
    case 0x41: ret = gb_ReadSTAT();  break;
    case 0x42: ret = gb_ReadSCY();   break;
    case 0x43: ret = gb_ReadSCX();   break;
    case 0x44: ret = gb_ReadLY();    break;
    case 0x45: ret = gb_ReadLYC();   break;
    case 0x46: ret = gb_ReadDMA();   break;
    case 0x47: ret = gb_ReadBGP();   break;
    case 0x48: ret = gb_ReadOBP0();  break;
    case 0x49: ret = gb_ReadOBP1();  break;
    case 0x4A: ret = gb_ReadWY();    break;
    case 0x4B: ret = gb_ReadWX();    break;
    case 0x4D: ret = gb_ReadKEY1();  break;
    case 0x4F: ret = gb_ReadVBK();   break;
    case 0x51: ret = gb_ReadHDMA1(); break;
    case 0x52: ret = gb_ReadHDMA2(); break;
    case 0x53: ret = gb_ReadHDMA3(); break;
    case 0x54: ret = gb_ReadHDMA4(); break;
    case 0x55: ret = gb_ReadHDMA5(); break;
    case 0x56: ret = gb_ReadRP();    break;
    case 0x68: ret = gb_ReadBCPS();  break;
    case 0x69: ret = gb_ReadBCPD();  break;
    case 0x6A: ret = gb_ReadOCPS();  break;
    case 0x6B: ret = gb_ReadOCPD();  break;
    case 0x70: ret = gb_ReadSVBK();  break;
    case 0xFF: ret = gb_ReadIE();    break;  
    default:   ret = g_IO[port];     break; // 0x80-0xFE: HRAM
    }

    return ret;
}

void gb_WriteByte(u16 offset, u8 byte)
{
    if      (offset < 0x8000) { gb_WriteROM(offset, byte); }
    else if (offset < 0xA000) { gb_WriteVRAM(offset, byte); }
    else if (offset < 0xC000) { gb_WriteSRAM(offset, byte); }
    else if (offset < 0xD000) { g_WRAM_lo[offset & 0x0FFF] = byte; }
    else if (offset < 0xE000) { g_WRAM_hi[offset & 0x0FFF] = byte; }
    else if (offset < 0xF000) { g_WRAM_lo[offset & 0x0FFF] = byte; }
    else if (offset < 0xFE00) { g_WRAM_hi[offset & 0x0FFF] = byte; }
    else if (offset < 0xFF00) { gb_WriteOAM(offset, byte); }
    else                      { gb_WriteIO(offset & 0xFF, byte); }
}

u8 gb_ReadByte(u16 offset)
{
    u8 ret;

    if      (offset < 0x8000) { ret = gb_ReadROM(offset); }
    else if (offset < 0xA000) { ret = gb_ReadVRAM(offset); }
    else if (offset < 0xC000) { ret = gb_ReadSRAM(offset); }
    else if (offset < 0xD000) { ret = g_WRAM_lo[offset & 0x0FFF]; }
    else if (offset < 0xE000) { ret = g_WRAM_hi[offset & 0x0FFF]; }
    else if (offset < 0xF000) { ret = g_WRAM_lo[offset & 0x0FFF]; }
    else if (offset < 0xFE00) { ret = g_WRAM_hi[offset & 0x0FFF]; }
    else if (offset < 0xFF00) { ret = gb_ReadOAM(offset); }
    else                      { ret = gb_ReadIO(offset & 0xFF); }
    
    return ret;
}
