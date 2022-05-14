//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

class gb_SweepUnit
{
private:
    s32  m_time;
    s32  m_direction;
    s32  m_shifts;
    s32  m_counter;
    s32  m_freq;
    s32  m_nextfreq;
    bool m_overflow;
    bool m_active;

    void Sweep();

public:
    gb_SweepUnit();

    void Reset();
    void SetSweepRegister(u8 NRX0);
    void Init(s32 frequency);
    bool Clock();
    s32  GetFrequency()     const;
    bool GetOverflowState() const;
};
