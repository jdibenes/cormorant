//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

class gb_SoundLength
{
private:
    s32  m_counter;
    s32  m_maxlength;
    bool m_off;
    bool m_active;

public:
    gb_SoundLength();

    void SetMaxLength(s32 maxlength);
    void SetCounter(s32 counter);
    void SetEnable(s32 enable);
    void Reset();
    void Init();
    void Clock();
    bool Expired() const;
};
