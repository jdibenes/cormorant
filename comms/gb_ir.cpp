//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "../cartridge/gb_pak.h"

static u8 g_RP;

void gb_ResetIR()
{
	g_RP = 0xFF;
}

void gb_WriteRP(u8 byte)
{
	g_RP = byte | 2;
}

u8 gb_ReadRP()
{
	return gb_IsCGB() ? (g_RP | 0x3E) : 0xFF;
}
