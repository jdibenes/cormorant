//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "gb_wave_pattern.h"

gb_WavePattern::gb_WavePattern()
{
    Reset();
}

void gb_WavePattern::Reset()
{
    m_wavetable.Reset();
    m_freq  = 0;
    m_ticks = 0;
    ReloadCounter();
}

void gb_WavePattern::WritePattern(u8 offset, s8 samples)
{
    m_wavetable.WriteWaveRAM(offset, samples);
}

u8 gb_WavePattern::ReadPattern(u8 offset)
{
    return m_wavetable.ReadWaveRAM(offset);
}

void gb_WavePattern::SetFrequency(s32 frequency)
{
    m_freq = frequency & 0x07FF;
}

void gb_WavePattern::Init()
{
    m_wavetable.Init();
    m_ticks = 0;
    ReloadCounter();
}

void gb_WavePattern::ReloadCounter()
{
    m_ticks += 2 * (2048 - m_freq);
}

void gb_WavePattern::Advance()
{
    ReloadCounter();
    m_wavetable.Clock();
}

void gb_WavePattern::Sync(s32 ticks)
{
    m_ticks -= ticks;
    while (m_ticks <= 0) { Advance(); }
}

s32 gb_WavePattern::GetSample() const
{
    return m_wavetable.GetSample();
}
