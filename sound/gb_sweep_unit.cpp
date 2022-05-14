//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "gb_sweep_unit.h"

gb_SweepUnit::gb_SweepUnit()
{
    Reset();
}

void gb_SweepUnit::Reset()
{
    m_time      = 0;
    m_direction = 1;
    m_shifts    = 0;
    m_counter   = 0;
    m_freq      = 0;
    m_nextfreq  = 0;
    m_overflow  = false;
    m_active    = false;
}

void gb_SweepUnit::SetSweepRegister(u8 NRX0)
{
    m_time      = BIT_FIELD(NRX0, 4, 7);
    m_direction = BIT_TEST(NRX0, 3) ? -1 : 1;
    m_shifts    = BIT_FIELD(NRX0, 0, 7);
}

void gb_SweepUnit::Sweep() {
    m_nextfreq = m_freq + (m_direction * (m_freq >> m_shifts));
    m_overflow = m_nextfreq > 0x7FF;
}

void gb_SweepUnit::Init(s32 frequency)
{
    m_freq     = frequency & 0x07FF;
    m_counter  = m_time;
    m_active   = m_time || m_shifts;
    m_overflow = false;
    if (m_shifts) { Sweep(); }
}

bool gb_SweepUnit::Clock()
{
    if (!m_time || !m_active || m_overflow) { return false; }
    m_counter = (m_counter - 1) & 7;
    if (m_counter) { return false; }
    m_counter = m_time;
    Sweep();
    if (m_overflow) { return false; }
    m_freq = m_nextfreq;
    Sweep();
    return true;
}

s32 gb_SweepUnit::GetFrequency() const
{
    return m_freq;
}

bool gb_SweepUnit::GetOverflowState() const
{
    return m_overflow;
}
