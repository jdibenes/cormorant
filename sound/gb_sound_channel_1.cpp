//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "gb_sound_channel_1.h"

gb_SoundChannel1::gb_SoundChannel1()
{
    m_length.SetMaxLength(64);
    Reset();
}

void gb_SoundChannel1::Reset()
{
    m_square.Reset();
    m_sweep.Reset();
    m_length.Reset();
    m_volume.Reset();

    m_freq = 0;
    m_off  = true;
}

void gb_SoundChannel1::WriteNR10(u8 NR10)
{
    m_sweep.SetSweepRegister(NR10);
}

void gb_SoundChannel1::WriteNR11(u8 NR11)
{
    m_square.SetDutyCycle(BIT_FIELD(NR11, 6, 3));
    m_length.SetCounter(64 - BIT_FIELD(NR11, 0, 63));
}

void gb_SoundChannel1::WriteNR12(u8 NR12)
{
    m_volume.SetVolumeEnvelopeRegister(NR12);
}

void gb_SoundChannel1::WriteNR13(u8 NR13)
{
    m_freq = (m_freq & 0x700) | NR13;
    m_square.SetFrequency(m_freq);
}

void gb_SoundChannel1::WriteNR14(u8 NR14)
{
    m_freq = (m_freq & 0xFF) | (BIT_FIELD(NR14, 0, 7) << 8);
    m_square.SetFrequency(m_freq);
    m_length.SetEnable(BIT_TEST(NR14, 6));
    if (!BIT_TEST(NR14, 7)) { return; }
    m_square.Init();
    m_sweep.Init(m_freq);
    m_length.Init();
    m_volume.Init();
    m_off = m_sweep.GetOverflowState();
}

void gb_SoundChannel1::ClockSweep()
{
    bool update = m_sweep.Clock();
    if (m_sweep.GetOverflowState()) { m_off = true; }
    if (!update) { return; }
    m_freq = m_sweep.GetFrequency();
    m_square.SetFrequency(m_freq);
}

void gb_SoundChannel1::ClockLength()
{
    m_length.Clock();
    if (m_length.Expired()) { m_off = true; }
}

void gb_SoundChannel1::ClockVolume()
{
    m_volume.Clock();
}

void gb_SoundChannel1::Sync(s32 ticks_square, s32 ticks_sweep, s32 ticks_length, s32 ticks_envelope)//s32 ticks)
{    
    while (ticks_envelope-- > 0) { ClockVolume(); }
    while (ticks_length--   > 0) { ClockLength(); }    
    while (ticks_sweep--    > 0) { ClockSweep();  }
    m_square.Sync(ticks_square);
}

s32 gb_SoundChannel1::GetSample() const
{
    return (m_square.GetSample() ? 1 : -1) * m_volume.GetVolume();
}

bool gb_SoundChannel1::IsChannelOff() const
{
    return m_off;
}
