//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "../gb_host.h"
#include "gb_sound.h"
#include "gb_sound_unit.h"

static s32 g_soundbufferclk;
static s32 g_soundbufferticks;

void gb_ResetSound()
{
    gb_ResetSoundUnit();
    ResetSoundBuffer();
    g_soundbufferticks = g_soundbufferclk;
}

static void gb_SendSoundSamples()
{
    SendSoundBufferSample(gb_GetSampleSOX(0), gb_GetSampleSOX(1));
    g_soundbufferticks += g_soundbufferclk;
}

static void gb_SyncSoundModules(s32 ticks)
{
    gb_SyncSoundUnit(ticks);
    g_soundbufferticks -= ticks;
    while (g_soundbufferticks <= 0) { gb_SendSoundSamples(); }
}

void gb_SyncSound(s32 ticks)
{
    for (s32 next_event = g_soundbufferticks; ticks >= next_event; next_event = g_soundbufferticks)
    {
        ticks -= next_event;
        gb_SyncSoundModules(next_event);
    }

    if (ticks > 0) { gb_SyncSoundModules(ticks); }
}

void gb_SetSoundPeriod(s32 clk)
{
    if (clk > 0) { g_soundbufferclk = clk; }
}
