//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "gb_sound_channel_3.h"

gb_SoundChannel3::gb_SoundChannel3()
{
    m_length.SetMaxLength(256);
    Reset();
}

void gb_SoundChannel3::Reset()
{
    m_wave.Reset();
    m_length.Reset();

    m_freq  = 0;
    m_level = 0;
    m_off   = true;
    m_dac   = false;
}

void gb_SoundChannel3::WriteNR30(u8 NR30)
{
    m_dac = BIT_TEST(NR30, 7) != 0;
    if (!m_dac) { m_off = true; }
}

void gb_SoundChannel3::WriteNR31(u8 NR31)
{
    m_length.SetCounter(256 - NR31);
}

void gb_SoundChannel3::WriteNR32(u8 NR32)
{
    m_level = BIT_FIELD(NR32, 5, 3);
}

void gb_SoundChannel3::WriteNR33(u8 NR33)
{
    m_freq = (m_freq & 0x700) | NR33;
    m_wave.SetFrequency(m_freq);
}

void gb_SoundChannel3::WriteNR34(u8 NR34)
{
    m_freq = (m_freq & 0xFF) | (BIT_FIELD(NR34, 0, 7) << 8);
    m_wave.SetFrequency(m_freq);
    m_length.SetEnable(BIT_TEST(NR34, 6));
    if (!BIT_TEST(NR34, 7)) { return; }
    m_wave.Init();
    m_length.Init();    
    m_off = !m_dac;
}

void gb_SoundChannel3::WriteWavePattern(u8 offset, s8 byte)
{
    m_wave.WritePattern(offset, byte);
}

u8 gb_SoundChannel3::ReadWavePattern(u8 offset)
{
    return m_wave.ReadPattern(offset);
}

void gb_SoundChannel3::ClockLength()
{
    m_length.Clock();
    if (m_length.Expired()) { m_off = true; }
}

void gb_SoundChannel3::Sync(s32 ticks, s32 ticks_length)
{
    while (ticks_length-- > 0) { ClockLength(); }
    m_wave.Sync(ticks);
}

s32 gb_SoundChannel3::GetSample() const
{
    return m_level != 0 ? ((2 * m_wave.GetSample()) - 15) >> (m_level - 1) : 0;
}

bool gb_SoundChannel3::IsChannelOff() const
{
    return m_off;
}











// Incorrecto
    //return m_level ? ((2 * (m_wave.GetSample() >> (m_level - 1))) - 15) : 0;
//s32 clkout[8], clkslen;
    //m_frame.Sync(ticks);

    //for (int i = 0; i < 8; i++) {clkout[i] = m_frame.DrawStepClocks(i);}
    //clkslen = m_frame.GetTicksLength();//clkout[0] + clkout[4] + clkout[2] + clkout[6];
    //m_frame.ClearTicks();

//m_wave.SetOutputEnable(NR30 & 0x80);
//if (m_wave.IsChannelOff()) {m_off = true;}
//m_off = 