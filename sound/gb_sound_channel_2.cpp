//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "gb_sound_channel_2.h"

gb_SoundChannel2::gb_SoundChannel2()
{
    m_length.SetMaxLength(64);
    Reset();
}

void gb_SoundChannel2::Reset()
{
    m_square.Reset();
    m_length.Reset();
    m_volume.Reset();

    m_freq = 0;
    m_off  = true;    
}

void gb_SoundChannel2::WriteNR21(u8 NR21)
{
    m_square.SetDutyCycle(BIT_FIELD(NR21, 6, 3));
    m_length.SetCounter(64 - BIT_FIELD(NR21, 0, 63));
}

void gb_SoundChannel2::WriteNR22(u8 NR22)
{
    m_volume.SetVolumeEnvelopeRegister(NR22);
}

void gb_SoundChannel2::WriteNR23(u8 NR23)
{
    m_freq = (m_freq & 0x0700) | NR23;
    m_square.SetFrequency(m_freq);
}

void gb_SoundChannel2::WriteNR24(u8 NR24)
{
    m_freq = (m_freq & 0xFF) | (BIT_FIELD(NR24, 0, 7) << 8);
    m_square.SetFrequency(m_freq);
    m_length.SetEnable(BIT_TEST(NR24, 6));
    if (!BIT_TEST(NR24, 7)) { return; }
    m_square.Init();
    m_length.Init();
    m_volume.Init();
    m_off = false;
}

void gb_SoundChannel2::ClockLength()
{
    m_length.Clock();
    if (m_length.Expired()) { m_off = true; }
}

void gb_SoundChannel2::ClockVolume()
{
    m_volume.Clock();
}

void gb_SoundChannel2::Sync(s32 ticks_square, s32 ticks_length, s32 ticks_envelope)
{
    while (ticks_envelope-- > 0) { ClockVolume(); }
    while (ticks_length--   > 0) { ClockLength(); }
    m_square.Sync(ticks_square);
}

s32 gb_SoundChannel2::GetSample() const
{
    return (m_square.GetSample() ? 1 : -1) * m_volume.GetVolume();
}

bool gb_SoundChannel2::IsChannelOff() const
{
    return m_off;
}
