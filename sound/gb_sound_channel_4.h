//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "gb_noise_pattern.h"
#include "gb_sound_length.h"
#include "gb_volume_envelope.h"

class gb_SoundChannel4
{
private:
    gb_NoisePattern   m_noise;
    gb_SoundLength    m_length;
    gb_VolumeEnvelope m_volume;

    bool m_off;

    void ClockLength();
    void ClockVolume();

public:
    gb_SoundChannel4();

    void Reset();
    void WriteNR41(u8 NR41);
    void WriteNR42(u8 NR42);
    void WriteNR43(u8 NR43);
    void WriteNR44(u8 NR44);
    void Sync(s32 ticks_noise, s32 ticks_length, s32 ticks_envelope);
    s32  GetSample()    const;
    bool IsChannelOff() const;
};
