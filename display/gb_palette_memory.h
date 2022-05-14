//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "../types.h"

class gb_PaletteMemory
{
private:
	v16 m_PD[8][4];
	u8  m_PS;

	u8 GetPaletteIndex()   const;
	u8 GetColorIndex()     const;
	u8 GetColorByteIndex() const;

public:
	gb_PaletteMemory();

	void Reset();
	void WritePS(u8 byte);
	u8   ReadPS() const;
	void WritePD(u8 byte);
	u8   ReadPD() const;

	u32  GetColor32(u8 palette, u8 color) const;
};
