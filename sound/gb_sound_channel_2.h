//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "gb_square_pattern.h"
#include "gb_sound_length.h"
#include "gb_volume_envelope.h"

class gb_SoundChannel2
{
private:
    gb_SquarePattern  m_square;
    gb_SoundLength    m_length;
    gb_VolumeEnvelope m_volume;

    s32  m_freq;
    bool m_off;

    void ClockLength();
    void ClockVolume();

public:
    gb_SoundChannel2();

    void Reset();
    void WriteNR21(u8 NR21);
    void WriteNR22(u8 NR22);
    void WriteNR23(u8 NR23);
    void WriteNR24(u8 NR24);
    void Sync(s32 ticks_square, s32 ticks_length, s32 ticks_envelope);
    s32  GetSample() const;
    bool IsChannelOff() const;
};
