//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

class gb_SquarePattern
{
private:
    static const s8 m_pattern[4][8];

    s32 m_dutycycle;
    s32 m_out;
    s32 m_freq;
    s32 m_ticks;
    
    void ReloadCounter();
    void Advance();

public:
    gb_SquarePattern();

    void Reset();
    void Init();
    void SetDutyCycle(s32 dutycycle);
    void SetFrequency(s32 frequency);
    void Sync(s32 ticks);
    s32  GetSample() const;
};
