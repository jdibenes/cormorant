//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "../cartridge/gb_pak.h"
#include "../core/gb_memory.h"
#include "gb_display.h"

static v16  g_addrsrc;
static v16  g_offsdst;
static s32  g_transferticks;
static bool g_enabled;
static s32  g_blocks;

void gb_ResetHDMA()
{
	g_addrsrc.w     = 0;
	g_offsdst.w     = 0;
	g_transferticks = 0;
	g_enabled       = false;
	g_blocks        = 0x7F;
}

static void gb_TransferBlock()
{
	for (int i = 0; i < 0x10; ++i) { gb_WriteVRAM(g_offsdst.w++, gb_ReadByte(g_addrsrc.w++)); }
	g_transferticks += 32;

	if (!(g_blocks--) || (g_offsdst.w >= 0x2000)) { g_enabled = false; }

	g_blocks    &= 0x7F;
	g_offsdst.w &= 0x1FFF;
}

static void gb_GeneralTransfer()
{
	while (g_enabled) { gb_TransferBlock(); }
}

void gb_HBlankTransfer()
{
	if (g_enabled) { gb_TransferBlock(); }
}

static void gb_BeginHDMA(u8 byte)
{
	g_enabled = true;
	g_blocks = BIT_FIELD(byte, 0, 0x7F);
	if (!BIT_TEST(byte, 7)) { gb_GeneralTransfer(); }
}

void gb_WriteHDMA1(u8 byte)
{
	g_addrsrc.b.hi = byte;
}

void gb_WriteHDMA2(u8 byte)
{
	g_addrsrc.b.lo = byte & 0xF0;
}

void gb_WriteHDMA3(u8 byte)
{
	g_offsdst.b.hi = byte & 0x1F;
}

void gb_WriteHDMA4(u8 byte)
{
	g_offsdst.b.lo = byte & 0xF0;
}

void gb_WriteHDMA5(u8 byte)
{
	if (!gb_IsCGB()) { return; }
	if (!g_enabled) { gb_BeginHDMA(byte); } else if (!BIT_TEST(byte, 7)) { g_enabled = false; }	
}

u8 gb_ReadHDMA1()
{
	return gb_IsCGB() ? g_addrsrc.b.hi : 0xFF;
}

u8 gb_ReadHDMA2()
{
	return gb_IsCGB() ? g_addrsrc.b.lo : 0xFF;
}

u8 gb_ReadHDMA3()
{
	return gb_IsCGB() ? g_offsdst.b.hi : 0xFF;
}

u8 gb_ReadHDMA4()
{
	return gb_IsCGB() ? g_offsdst.b.lo : 0xFF;
}

u8 gb_ReadHDMA5()
{
	return gb_IsCGB() ? ((g_enabled ? 0 : 0x80) | (g_blocks & 0x7F)) : 0xFF;
}

s32 gb_SyncHDMA()
{
	s32 ticks = g_transferticks;
	g_transferticks = 0;
	return ticks;
}
