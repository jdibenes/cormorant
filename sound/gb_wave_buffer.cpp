//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "gb_wave_buffer.h"

gb_WaveBuffer::gb_WaveBuffer()
{
    Reset();
}

void gb_WaveBuffer::Reset()
{
    m_position = 0;
    for (int i = 0; i < 16; i++) { m_pattern[i] = 0; }
}

void gb_WaveBuffer::WriteWaveRAM(u8 offset, s8 byte)
{
    m_pattern[offset & 0x0F] = byte;
}

u8 gb_WaveBuffer::ReadWaveRAM(u8 offset)
{
    return m_pattern[offset & 0x0F];
}

void gb_WaveBuffer::Init()
{
    m_position = 0;
}

void gb_WaveBuffer::Clock()
{
    m_position = (m_position + 1) & 0x1F;
}

s32 gb_WaveBuffer::GetSample() const
{
    return (BIT_TEST(m_position, 0) ? m_pattern[m_position >> 1] : (m_pattern[m_position >> 1] >> 4)) & 0x0F;
}
