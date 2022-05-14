//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

class gb_FrameSequencer
{
private:
    s32 m_ticks;
    s32 m_step;
    s32 m_clklist[8];

    void ReloadCounter();
    void NextStep();

public:
    gb_FrameSequencer();

    void Reset();
    void Sync(s32 ticks);
    s32  GetTicksSweep()    const;
    s32  GetTicksLength()   const;
    s32  GetTicksEnvelope() const;
    void ClearTicks();
};
