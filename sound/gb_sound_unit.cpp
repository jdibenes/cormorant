//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "gb_frame_sequencer.h"
#include "gb_sound_channel_1.h"
#include "gb_sound_channel_2.h"
#include "gb_sound_channel_3.h"
#include "gb_sound_channel_4.h"

static gb_FrameSequencer g_frame;

static gb_SoundChannel1 g_sc1;
static gb_SoundChannel2 g_sc2;
static gb_SoundChannel3 g_sc3;
static gb_SoundChannel4 g_sc4;

static s32  g_SOXvol[2];
static s32  g_SOXcnt[2];
static bool g_soundon;

static u8 g_NR10;
static u8 g_NR11;
static u8 g_NR12;
static u8 g_NR13;
static u8 g_NR14;
static u8 g_NR21;
static u8 g_NR22;
static u8 g_NR23;
static u8 g_NR24;
static u8 g_NR30;
static u8 g_NR31;
static u8 g_NR32;
static u8 g_NR33;
static u8 g_NR34;
static u8 g_NR41;
static u8 g_NR42;
static u8 g_NR43;
static u8 g_NR44;
static u8 g_NR50;
static u8 g_NR51;

void gb_ResetSoundUnit()
{
    g_frame.Reset();

    g_sc1.Reset();
    g_sc2.Reset();
    g_sc3.Reset();
    g_sc4.Reset();

    g_SOXvol[0] = 0;
    g_SOXvol[1] = 0;
    g_SOXcnt[0] = 0;
    g_SOXcnt[1] = 0;
    g_soundon   = true;

    g_NR10 = 0x80;
    g_NR11 = 0xBF;
    g_NR12 = 0xF3;
    g_NR13 = 0xFF;
    g_NR14 = 0xBF;
    g_NR21 = 0x3F;
    g_NR22 = 0x00;
    g_NR23 = 0xFF;
    g_NR24 = 0xBF;
    g_NR30 = 0x7F;
    g_NR31 = 0xFF;
    g_NR32 = 0x9F;
    g_NR33 = 0xFF;
    g_NR34 = 0xBF;
    g_NR41 = 0xFF;
    g_NR42 = 0x00;
    g_NR43 = 0x00;
    g_NR44 = 0xBF;
    g_NR50 = 0x77;
    g_NR51 = 0xF3;
}

void gb_SyncSoundUnit(s32 ticks)
{
    g_frame.Sync(ticks);
    s32 ticks_sweep    = g_frame.GetTicksSweep();
    s32 ticks_length   = g_frame.GetTicksLength();
    s32 ticks_envelope = g_frame.GetTicksEnvelope();
    g_frame.ClearTicks();

    g_sc1.Sync(ticks, ticks_sweep, ticks_length, ticks_envelope);
    g_sc2.Sync(ticks, ticks_length, ticks_envelope);
    g_sc3.Sync(ticks, ticks_length);
    g_sc4.Sync(ticks, ticks_length, ticks_envelope);
}

s32 gb_GetSampleSOX(s32 terminal)
{
    s32 sample;
    s32 sox;
    s32 soxcnt;

    if (!g_soundon) { return 0; }

    sample = 0;
    sox    = terminal & 1;
    soxcnt = g_SOXcnt[sox];

    if (!g_sc1.IsChannelOff() && BIT_TEST(soxcnt, 0)) { sample += g_sc1.GetSample(); }
    if (!g_sc2.IsChannelOff() && BIT_TEST(soxcnt, 1)) { sample += g_sc2.GetSample(); }
    if (!g_sc3.IsChannelOff() && BIT_TEST(soxcnt, 2)) { sample += g_sc3.GetSample(); }
    if (!g_sc4.IsChannelOff() && BIT_TEST(soxcnt, 3)) { sample += g_sc4.GetSample(); }

    return sample * (g_SOXvol[sox] + 1);
}

static u8 gb_GetChannelStatus()
{
    u8 flags;

    if (!g_soundon) { return 0; }

    flags = 0x80;
    if (!g_sc1.IsChannelOff()) { flags |= 1; }
    if (!g_sc2.IsChannelOff()) { flags |= 2; }
    if (!g_sc3.IsChannelOff()) { flags |= 4; }
    if (!g_sc4.IsChannelOff()) { flags |= 8; }

    return flags;
}

void gb_WriteNR10(u8 byte)
{
    g_NR10 = byte;
    g_sc1.WriteNR10(byte);
}

void gb_WriteNR11(u8 byte)
{
    g_NR11 = byte;
    g_sc1.WriteNR11(byte);
}

void gb_WriteNR12(u8 byte)
{
    g_NR12 = byte;
    g_sc1.WriteNR12(byte);
}

void gb_WriteNR13(u8 byte)
{
    g_NR13 = byte;
    g_sc1.WriteNR13(byte);
}

void gb_WriteNR14(u8 byte)
{
    g_NR14 = byte;
    g_sc1.WriteNR14(byte);
}

void gb_WriteNR21(u8 byte)
{
    g_NR21 = byte;
    g_sc2.WriteNR21(byte);
}

void gb_WriteNR22(u8 byte)
{
    g_NR22 = byte;
    g_sc2.WriteNR22(byte);
}

void gb_WriteNR23(u8 byte)
{
    g_NR23 = byte;
    g_sc2.WriteNR23(byte);
}

void gb_WriteNR24(u8 byte)
{
    g_NR24 = byte;
    g_sc2.WriteNR24(byte);
}

void gb_WriteNR30(u8 byte)
{
    g_NR30 = byte;
    g_sc3.WriteNR30(byte);
}

void gb_WriteNR31(u8 byte)
{
    g_NR31 = byte;
    g_sc3.WriteNR31(byte);
}

void gb_WriteNR32(u8 byte)
{
    g_NR32 = byte;
    g_sc3.WriteNR32(byte);
}

void gb_WriteNR33(u8 byte)
{
    g_NR33 = byte;
    g_sc3.WriteNR33(byte);
}

void gb_WriteNR34(u8 byte)
{
    g_NR34 = byte;
    g_sc3.WriteNR34(byte);
}

void gb_WriteNR41(u8 byte)
{
    g_NR41 = byte;
    g_sc4.WriteNR41(byte);
}

void gb_WriteNR42(u8 byte)
{
    g_NR42 = byte;
    g_sc4.WriteNR42(byte);
}

void gb_WriteNR43(u8 byte)
{
    g_NR43 = byte;
    g_sc4.WriteNR43(byte);
}

void gb_WriteNR44(u8 byte)
{
    g_NR44 = byte;
    g_sc4.WriteNR44(byte);
}

void gb_WriteNR50(u8 byte)
{
    g_NR50 = byte;

    g_SOXvol[0] = BIT_FIELD(byte, 0, 7);
    g_SOXvol[1] = BIT_FIELD(byte, 4, 7);
}

void gb_WriteNR51(u8 byte)
{
    g_NR51 = byte;

    g_SOXcnt[0] = BIT_FIELD(byte, 0, 0xF);
    g_SOXcnt[1] = BIT_FIELD(byte, 4, 0xF);
}

void gb_WriteNR52(u8 byte)
{
    bool sndon = BIT_TEST(byte, 7);
    if (!(g_soundon ^ sndon)) { return; }

    g_sc1.Reset();
    g_sc2.Reset();
    g_sc3.Reset();
    g_sc4.Reset();

    g_soundon = sndon;
}

void gb_WriteWaveRAM(u8 port, u8 byte)
{
    g_sc3.WriteWavePattern(port, byte);
}

u8 gb_ReadNR10()
{
    return g_NR10;
}

u8 gb_ReadNR11()
{
    return g_NR11;
}

u8 gb_ReadNR12()
{
    return g_NR12;
}

u8 gb_ReadNR13()
{
    return g_NR13;
}

u8 gb_ReadNR14()
{
    return g_NR14;
}

u8 gb_ReadNR21()
{
    return g_NR21;
}

u8 gb_ReadNR22()
{
    return g_NR22;
}

u8 gb_ReadNR23()
{
    return g_NR23;
}

u8 gb_ReadNR24()
{
    return g_NR24;
}

u8 gb_ReadNR30()
{
    return g_NR30;
}

u8 gb_ReadNR31()
{
    return g_NR31;
}

u8 gb_ReadNR32()
{
    return g_NR32;
}

u8 gb_ReadNR33()
{
    return g_NR33;
}

u8 gb_ReadNR34()
{
    return g_NR34;
}

u8 gb_ReadNR41()
{
    return g_NR41;
}

u8 gb_ReadNR42()
{
    return g_NR42;
}

u8 gb_ReadNR43()
{
    return g_NR43;
}

u8 gb_ReadNR44()
{
    return g_NR44;
}

u8 gb_ReadNR50()
{
    return g_NR50;
}

u8 gb_ReadNR51()
{
    return g_NR51;
}

u8 gb_ReadNR52()
{
    return gb_GetChannelStatus() | 0x70;
}

u8 gb_ReadWaveRAM(u8 port)
{
    return g_sc3.ReadWavePattern(port);
}
