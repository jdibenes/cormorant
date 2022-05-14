//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "gb_wave_pattern.h"
#include "gb_sound_length.h"

class gb_SoundChannel3
{
private:
    gb_WavePattern m_wave;
    gb_SoundLength m_length;

    s32  m_freq;
    s32  m_level;
    bool m_off;
    bool m_dac;

    void ClockLength();

public:
    gb_SoundChannel3();

    void Reset();
    void WriteNR30(u8 NR30);
    void WriteNR31(u8 NR31);
    void WriteNR32(u8 NR32);
    void WriteNR33(u8 NR33);
    void WriteNR34(u8 NR34);
    void WriteWavePattern(u8 offset, s8 byte);
    u8   ReadWavePattern(u8 offset);
    void Sync(s32 ticks, s32 ticks_length);
    s32  GetSample()    const;
    bool IsChannelOff() const;
};
