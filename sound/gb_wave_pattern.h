//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "gb_wave_buffer.h"

class gb_WavePattern
{
private:
    gb_WaveBuffer m_wavetable;
    s32 m_ticks;
    s32 m_freq;

    void ReloadCounter();
    void Advance();

public:
    gb_WavePattern();

    void Reset();
    void WritePattern(u8 offset, s8 byte);
    u8   ReadPattern(u8 offset);
    void SetFrequency(s32 frequency);
    void Init();
    void Sync(s32 ticks);
    s32  GetSample() const;
};
