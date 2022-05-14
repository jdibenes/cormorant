//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "gb_square_pattern.h"
#include "gb_sweep_unit.h"
#include "gb_sound_length.h"
#include "gb_volume_envelope.h"

class gb_SoundChannel1
{
private:
    gb_SquarePattern  m_square;
    gb_SweepUnit      m_sweep;
    gb_SoundLength    m_length;
    gb_VolumeEnvelope m_volume;

    s32  m_freq;
    bool m_off;

    void ClockSweep();
    void ClockLength();
    void ClockVolume();

public:
    gb_SoundChannel1();

    void Reset();
    void WriteNR10(u8 NR10);
    void WriteNR11(u8 NR11);
    void WriteNR12(u8 NR12);
    void WriteNR13(u8 NR13);
    void WriteNR14(u8 NR14);
    void Sync(s32 ticks_square, s32 ticks_sweep, s32 ticks_length, s32 ticks_envelope);
    s32  GetSample()    const;
    bool IsChannelOff() const;
};
