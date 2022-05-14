//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

class gb_NoisePattern
{
private:
    static const u32 m_pattern[2][2];
    static const s32 m_period[8];

    u32 m_shiftreg;
    s32 m_shiftclk;
    s32 m_width;
    s32 m_ratio;
    s32 m_ticks;

    void ReloadCounter();
    void Shift();

public:
    gb_NoisePattern();

    void Reset();
    void SetNoiseRegister(u8 NR43);
    void Init();
    void Sync(s32 ticks);
    s32  GetSample() const;
};
