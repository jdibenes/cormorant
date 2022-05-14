//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include <Windows.h>
#include "../gb_host.h"

// Factors 16,3,7,11,19
#define SYNC_TIME (70224 / (19*3*7))

static s32           g_ticks;
static bool          g_enable;
static LARGE_INTEGER g_PF;
static LARGE_INTEGER g_PC_start;

void OnBeginEmulation()
{
    g_enable = QueryPerformanceFrequency(&g_PF) && QueryPerformanceCounter(&g_PC_start);
    g_ticks = 0;
}

void SyncHost(s32 ticks)
{
    if (!g_enable) { return; }

    g_ticks += ticks;
    if (g_ticks < SYNC_TIME) { return; }

    double elapsed_guest = double(g_ticks) / double(GB_CLK);
    double elapsed_host;

    LARGE_INTEGER pf;

    do {
        if (!QueryPerformanceCounter(&pf)) 
        {
            g_enable = false;
            return;
        }

        if (pf.QuadPart < g_PC_start.QuadPart)
        {
            g_PC_start = pf;
            g_ticks    = 0;
            return;
        }

        elapsed_host = double(pf.QuadPart - g_PC_start.QuadPart) / double(g_PF.QuadPart);
    }
    while (elapsed_guest > elapsed_host);

    g_PC_start = pf;
    g_ticks   -= (s32)(elapsed_host * GB_CLK);
}
