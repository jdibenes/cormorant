//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "gb_square_pattern.h"

const s8 gb_SquarePattern::m_pattern[4][8] =
{
    {0, 0, 0, 0, 0, 0, 0, 1}, // 12.5%
    {1, 0, 0, 0, 0, 0, 0, 1}, // 25%
    {1, 0, 0, 0, 0, 1, 1, 1}, // 50%
    {0, 1, 1, 1, 1, 1, 1, 0}  // 75%
};

gb_SquarePattern::gb_SquarePattern()
{
    Reset();
}

void gb_SquarePattern::Reset()
{
    m_dutycycle = 0;
    m_out       = 0;
    m_freq      = 0;
    m_ticks     = 0;
    ReloadCounter();
}

void gb_SquarePattern::Init()
{
    m_ticks = 0;
    ReloadCounter();
}

void gb_SquarePattern::SetDutyCycle(s32 dutycycle)
{
    m_dutycycle = dutycycle & 3;
}

void gb_SquarePattern::SetFrequency(s32 frequency)
{
    m_freq = frequency & 0x07FF;
}

void gb_SquarePattern::ReloadCounter()
{
    m_ticks += 4 * (2048 - m_freq);
}

void gb_SquarePattern::Advance()
{
    m_out = (m_out + 1) & 7;
    ReloadCounter();
}

void gb_SquarePattern::Sync(s32 ticks)
{
    m_ticks -= ticks;
    while (m_ticks <= 0) { Advance(); }
}

s32 gb_SquarePattern::GetSample() const
{
    return m_pattern[m_dutycycle][m_out];
}
