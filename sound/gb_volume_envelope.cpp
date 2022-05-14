//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "gb_volume_envelope.h"

gb_VolumeEnvelope::gb_VolumeEnvelope()
{
    Reset();
}

void gb_VolumeEnvelope::Reset()
{
    m_initvol   = 0;
    m_direction = -1;
    m_step      = 0;
    m_active    = false;
    m_counter   = 0;
    m_vol       = 0;
}

void gb_VolumeEnvelope::SetVolumeEnvelopeRegister(u8 NRX2)
{
    m_initvol   = BIT_FIELD(NRX2, 4, 0xF);
    m_direction = BIT_TEST(NRX2, 3) ? 1 : -1;
    m_step      = BIT_FIELD(NRX2, 0, 7);
}

void gb_VolumeEnvelope::Init()
{
    m_active  = true;
    m_counter = m_step;
    m_vol     = m_initvol;
}

void gb_VolumeEnvelope::Step()
{
    s32 nextvol;
    nextvol = m_vol + m_direction;
    if (nextvol < 0 || nextvol > 15) { m_active = false; } else { m_vol = nextvol; }
}

void gb_VolumeEnvelope::Clock()
{
    if (!m_step || !m_active) { return; }
    m_counter = (m_counter - 1) & 7;
    if (m_counter) { return; }
    m_counter = m_step;
    Step();
}

s32 gb_VolumeEnvelope::GetVolume() const
{
    return m_vol;
}
