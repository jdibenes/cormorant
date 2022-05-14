//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

class gb_WaveBuffer
{
private:
    s8  m_pattern[16];
    s32 m_position;

public:
    gb_WaveBuffer();

    void Reset();
    void WriteWaveRAM(u8 offset, s8 byte);
    u8   ReadWaveRAM(u8 offset);
    void Init();
    void Clock();
    s32  GetSample() const;
};
