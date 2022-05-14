//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "../cartridge/gb_pak.h"
#include "../core/gb_memory.h"
#include "gb_display.h"

static u8 g_DMA;

void gb_ResetDMA()
{
	g_DMA = gb_IsCGB() ? 0x00 : 0xFF;
}

void gb_WriteDMA(u8 byte)
{
	v16 srcaddr;

	srcaddr.b.hi = byte;
	srcaddr.b.lo = 0x00;

	g_DMA = byte;

	for (u16 x = 0; x < 0xA0; x++) { gb_WriteOAM(x, gb_ReadByte(srcaddr.w + x)); }
}

u8 gb_ReadDMA()
{
	return g_DMA;
}
