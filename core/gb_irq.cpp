//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "gb_core.h"

static u8 g_IF;
static u8 g_IE;

void gb_ResetIRQ()
{
	g_IF = 0xE1;
	g_IE = 0;
}

void gb_WriteIF(u8 byte)
{
	g_IF = byte;
}

void gb_WriteIE(u8 byte)
{
	g_IE = byte;
}

u8 gb_ReadIF()
{
	return g_IF;
}

u8 gb_ReadIE()
{
	return g_IE;
}

u8 gb_IRQsPending()
{
	return ((g_IF & g_IE) & 0x1F);
}

void gb_RequestInterrupt(u8 IRQ)
{
	g_IF |= IRQ;
}

void gb_AcknowledgeInterrupt(u8 IRQ)
{
	g_IF &= ~IRQ;
}
