//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include <memory.h>
#include "gb_palette_memory.h"

gb_PaletteMemory::gb_PaletteMemory()
{
	Reset();
}

void gb_PaletteMemory::Reset()
{
	memset(m_PD, 0xFF, sizeof(m_PD));
	m_PS = 0;
}

void gb_PaletteMemory::WritePS(u8 byte)
{
	m_PS = byte;
}

u8 gb_PaletteMemory::ReadPS() const
{
	return m_PS | 0x40;
}

u8 gb_PaletteMemory::GetPaletteIndex() const
{
	return BIT_FIELD(m_PS, 3, 7);
}

u8 gb_PaletteMemory::GetColorIndex() const
{
	return BIT_FIELD(m_PS, 1, 3);
}

u8 gb_PaletteMemory::GetColorByteIndex() const
{
	return BIT_FIELD(m_PS, 0, 1);
}

void gb_PaletteMemory::WritePD(u8 byte)
{
	u8 palette = GetPaletteIndex();
	u8 color = GetColorIndex();
	if (GetColorByteIndex()) { m_PD[palette][color].b.hi = byte; } else { m_PD[palette][color].b.lo = byte; }
	if (BIT_TEST(m_PS, 7)) { m_PS = (m_PS & ~0x3F) | ((m_PS + 1) & 0x3F); }
}

u8 gb_PaletteMemory::ReadPD() const
{
	u8 palette = GetPaletteIndex();
	u8 color = GetColorIndex();
	return GetColorByteIndex() ? m_PD[palette][color].b.hi : m_PD[palette][color].b.lo;
}

u32 gb_PaletteMemory::GetColor32(u8 palette, u8 color) const
{
	u32 color16 = m_PD[palette & 7][color & 3].w;
	v32 color32;

	color32.w.lo.b.lo = BIT_FIELD(color16, 10, 0x1F) << 3;
	color32.w.lo.b.hi = BIT_FIELD(color16,  5, 0x1F) << 3;
	color32.w.hi.b.lo = BIT_FIELD(color16,  0, 0x1F) << 3;
	color32.w.hi.b.hi = 0xFF;

	return color32.d;
}
