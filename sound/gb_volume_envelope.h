//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

class gb_VolumeEnvelope
{
private:
    s32  m_initvol;
    s32  m_vol;
    s32  m_direction;
    s32  m_step;
    s32  m_counter;
    bool m_active;

    void Step();

public:
    gb_VolumeEnvelope();

    void Reset();
    void SetVolumeEnvelopeRegister(u8 NRX2);
    void Init();
    void Clock();
    s32  GetVolume() const;
};
