//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "gb_sound_channel_4.h"

gb_SoundChannel4::gb_SoundChannel4()
{
    m_length.SetMaxLength(64);
    Reset();
}

void gb_SoundChannel4::Reset()
{
    m_noise.Reset();
    m_length.Reset();
    m_volume.Reset();
    
    m_off = true;
}

void gb_SoundChannel4::WriteNR41(u8 NR41)
{
    m_length.SetCounter(64 - BIT_FIELD(NR41, 0, 63));
}

void gb_SoundChannel4::WriteNR42(u8 NR42)
{
    m_volume.SetVolumeEnvelopeRegister(NR42);
}

void gb_SoundChannel4::WriteNR43(u8 NR43)
{
    m_noise.SetNoiseRegister(NR43);
}

void gb_SoundChannel4::WriteNR44(u8 NR44)
{
    m_length.SetEnable(BIT_TEST(NR44, 6));
    if (!BIT_TEST(NR44, 7)) { return; }
    m_noise.Init();
    m_length.Init();
    m_volume.Init();
    m_off = false;
}

void gb_SoundChannel4::ClockLength()
{
    m_length.Clock();
    if (m_length.Expired()) { m_off = true; }
}

void gb_SoundChannel4::ClockVolume()
{
    m_volume.Clock();
}

void gb_SoundChannel4::Sync(s32 ticks_noise, s32 ticks_length, s32 ticks_envelope)
{
    while (ticks_envelope-- > 0) { ClockVolume(); }
    while (ticks_length--   > 0) { ClockLength(); }
    m_noise.Sync(ticks_noise);
}

s32 gb_SoundChannel4::GetSample() const
{
    return (m_noise.GetSample() ? 1 : -1) * m_volume.GetVolume();
}

bool gb_SoundChannel4::IsChannelOff() const
{
    return m_off;
}
