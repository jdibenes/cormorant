//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "gb_noise_pattern.h"

const u32 gb_NoisePattern::m_pattern[2][2] = { {0x3FFF, 0x3F}, {14, 6} };
const s32 gb_NoisePattern::m_period[8]     = { 8, 16, 32, 48, 64, 80, 96, 112 };

gb_NoisePattern::gb_NoisePattern()
{
    Reset();
}

void gb_NoisePattern::Reset()
{
    m_shiftreg = 1;
    m_ratio    = 0;
    m_shiftclk = 0;
    m_width    = 0;
    m_ticks    = 0;
    ReloadCounter();
}

void gb_NoisePattern::SetNoiseRegister(u8 NR43)
{
    m_shiftclk = BIT_FIELD(NR43, 4, 0xF);
    m_width    = BIT_TEST(NR43, 8) ? 1 : 0;
    m_ratio    = BIT_FIELD(NR43, 0, 7);
}

void gb_NoisePattern::Init()
{
    m_shiftreg = 0x7FFF;
    m_ticks    = 0;
    ReloadCounter();
}

void gb_NoisePattern::ReloadCounter()
{
    m_ticks += m_period[m_ratio] << m_shiftclk;
}

void gb_NoisePattern::Shift()
{
    s32 sh = m_shiftreg >> 1;
    m_shiftreg = (sh & m_pattern[0][m_width]) | (((m_shiftreg ^ sh) & 1) << m_pattern[1][m_width]);
    ReloadCounter();
}

void gb_NoisePattern::Sync(s32 ticks)
{
    m_ticks -= ticks;
    while (m_ticks <= 0) { Shift(); }
}

s32 gb_NoisePattern::GetSample() const
{
    return (~m_shiftreg) & 1;
}
