//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "gb_frame_sequencer.h"

gb_FrameSequencer::gb_FrameSequencer()
{
    Reset();
}

void gb_FrameSequencer::Reset()
{
    m_ticks = 0;
    m_step = 0;
    ClearTicks();
    ReloadCounter();
    m_ticks /= 2;
}

void gb_FrameSequencer::ReloadCounter()
{
    m_ticks += 8192;
}

void gb_FrameSequencer::NextStep()
{
    ++(m_clklist[m_step]);
    ReloadCounter();
    m_step = (m_step + 1) & 7;
}

void gb_FrameSequencer::Sync(s32 ticks)
{
    m_ticks -= ticks;
    while (m_ticks <= 0) { NextStep(); }
}

s32 gb_FrameSequencer::GetTicksSweep() const
{
    return m_clklist[2] + m_clklist[6];
}

s32 gb_FrameSequencer::GetTicksLength() const
{
    return m_clklist[0] + m_clklist[2] + m_clklist[4] + m_clklist[6];
}

s32 gb_FrameSequencer::GetTicksEnvelope() const
{
    return m_clklist[7];
}

void gb_FrameSequencer::ClearTicks()
{
    for (int i = 0; i < 8; i++) { m_clklist[i] = 0; }
}
