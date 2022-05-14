//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "gb_sound_length.h"

gb_SoundLength::gb_SoundLength()
{
    m_maxlength = 0;
    Reset();
}

void gb_SoundLength::SetMaxLength(s32 maxlength)
{
    m_maxlength = maxlength;
}

void gb_SoundLength::Reset()
{
    m_counter = 0;
    m_active  = false;
    m_off     = true;
}

void gb_SoundLength::SetCounter(s32 counter)
{
    m_counter = counter;
}

void gb_SoundLength::SetEnable(s32 enable)
{
    m_active = enable != 0;
}

void gb_SoundLength::Init()
{
    m_off = false;
    if (!m_counter) { m_counter = m_maxlength; }
}

void gb_SoundLength::Clock()
{
    if (m_counter <= 0 || !m_active) { return; }
    --m_counter;
    if (!m_counter) { m_off = true; }
}

bool gb_SoundLength::Expired() const
{
    return m_off;
}
