//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "../cartridge/gb_pak.h"

static u8 g_KEY1;

void gb_ResetSpeed()
{
    g_KEY1 = 0;
}

void gb_WriteKEY1(u8 byte)
{
    if (!gb_IsCGB()) { return; }
    g_KEY1 = (g_KEY1 & 0x80) | (byte & 0x7F);
}

u8 gb_ReadKEY1()
{
    return gb_IsCGB() ? (g_KEY1 | 0x7E) : 0xFF;
}

bool gb_CGBSpeed()
{
    return BIT_TEST(g_KEY1, 7);
}

void gb_CGBSetSpeed()
{
    if (!gb_IsCGB()) { return; }
    if (BIT_TEST(g_KEY1, 0)) { g_KEY1 = g_KEY1 ^ 0x80; }
    g_KEY1 &= 0xFE;
}
