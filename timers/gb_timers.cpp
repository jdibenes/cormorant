//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "../core/gb_core.h"

static const s32 g_timerclk[4] = { 1024, 16, 64, 256 };

static u16 g_DIV;
static u8  g_TIMA;
static u8  g_TMA;
static u8  g_TAC;
static s32 g_timerticks[4];

static void gb_ResetTicks()
{
    for (int i = 0; i < 4; ++i) { g_timerticks[i] = g_timerclk[i]; }
}

static void gb_ResetDIV()
{
    g_DIV = 0;
    gb_ResetTicks();
}

void gb_ResetTimers()
{
    gb_ResetDIV();

    g_TIMA = 0;
    g_TMA  = 0;
    g_TAC  = 0xF8;
}

void gb_WriteDIV(u8 byte)
{
    gb_ResetDIV();
}

void gb_WriteTIMA(u8 byte)
{
    g_TIMA = byte;
}

void gb_WriteTMA(u8 byte)
{
    g_TMA = byte;
}

void gb_WriteTAC(u8 byte)
{
    g_TAC = byte;
}

u8 gb_ReadDIV()
{
    return BIT_FIELD(g_DIV, 8, 0xFF);
}

u8 gb_ReadTIMA()
{
    return g_TIMA;
}

u8 gb_ReadTMA()
{
    return g_TMA;
}

u8 gb_ReadTAC()
{
    return g_TAC | 0xF8;
}

static void gb_SyncDIV(s32 ticks)
{
    g_DIV += ticks;
}

static void gb_IncrementTIMA()
{
    if (++g_TIMA) { return; }
    g_TIMA = g_TMA;
    gb_RequestInterrupt(IRQ_TMR);
}

static s32 gb_SyncTIMAFromDIV(s32 id, s32 ticks)
{
    s32 clk = 0;
    g_timerticks[id] -= ticks;
    for (; g_timerticks[id] <= 0; g_timerticks[id] += g_timerclk[id]) { clk++; }
    return clk;
}

static void gb_SyncTIMA(s32 ticks)
{
    s32 clks[4];
    for (int i = 0; i < 4; ++i) { clks[i] = gb_SyncTIMAFromDIV(i, ticks); }
    if (!BIT_TEST(g_TAC, 2)) { return; }
    for (s32 clk = 0; clk < clks[BIT_FIELD(g_TAC, 0, 3)]; ++clk) { gb_IncrementTIMA(); }
}

void gb_SyncTimers(s32 ticks) 
{
    gb_SyncDIV(ticks);
    gb_SyncTIMA(ticks);
}
