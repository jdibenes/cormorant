//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "../core/gb_core.h"
#include "../cartridge/gb_pak.h"

static u8   g_SB;
static u8   g_SC;
static bool g_enabled;
static s32  g_steps;
static s32  g_linkclk;
static s32  g_linkticks;

static void gb_ReloadSerialCounter()
{
    g_linkticks += g_linkclk;
}

void gb_ResetSerial()
{
    g_SB        = 0;
    g_SC        = gb_IsCGB() ? 0x7F : 0x7E;
    g_enabled   = false;
    g_steps     = 8;
    g_linkclk   = 512;
    g_linkticks = 0;
    gb_ReloadSerialCounter();
}

void gb_WriteSB(u8 byte)
{
    g_SB = byte;
}

void gb_WriteSC(u8 byte)
{
    g_SC = byte;
    if (!BIT_TEST(g_SC, 7) || !BIT_TEST(g_SC, 0)) { return; }
    g_enabled   = true;
    g_steps     = 8;
    g_linkclk   = (gb_IsCGB() && BIT_TEST(g_SC, 1)) ? 16 : 512;
    g_linkticks = 0;
    gb_ReloadSerialCounter();
}

u8 gb_ReadSB()
{
    return g_SB;
}

u8 gb_ReadSC()
{
    return g_SC | (gb_IsCGB() ? 0x7C : 0x7E);
}

static bool gb_ReceiveOpen()
{
    g_SB = (g_SB << 1) | 1;
    gb_ReloadSerialCounter();
    if (--g_steps) { return true; }
    g_SC &= 0x7F;
    g_enabled = false;
    gb_RequestInterrupt(IRQ_LNK);
    return false;
}

void gb_SyncSerial(s32 ticks)
{
    if (!g_enabled) { return; }
    g_linkticks -= ticks;
    while ((g_linkticks <= 0) && gb_ReceiveOpen());
}
