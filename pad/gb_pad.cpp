//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "../gb_host.h"
#include "gb_pad.h"
#include "../core/gb_core.h"

static u8 g_P1;

void gb_ResetJoypad()
{
    g_P1 = 0xCF;
}

void gb_WriteP1(u8 byte)
{
    g_P1 = (byte & 0x30) | (g_P1 & 0x0F);
}

u8 gb_ReadP1()
{
    return g_P1 | 0xC0;
}

void gb_SyncJoypad()
{
    u8   value   =   g_P1 | 0x0F;
    bool selbtn  = !(g_P1 & 0x20);
    bool seldpad = !(g_P1 & 0x10);
    bool a;
    bool b;
    bool select;
    bool start;
    bool right;
    bool left;
    bool up;
    bool down;
    u8   maskbtn;
    u8   maskdpad;

    if (selbtn || seldpad) { ReadJoypad(a, b, select, start, right, left, up, down); }

    maskbtn  = selbtn  ? ((a     << 0) | (b    << 1) | (select << 2) | (start << 3)) : 0;
    maskdpad = seldpad ? ((right << 0) | (left << 1) | (up     << 2) | (down  << 3)) : 0;

    value &= ~(maskbtn | maskdpad);
    if ((~value & g_P1) & 0x0F) { gb_RequestInterrupt(IRQ_PAD); }
    g_P1 = value;
}
