//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include <memory.h>
#include "../gb_host.h"
#include "../core/gb_core.h"
#include "../cartridge/gb_pak.h"
#include "gb_palette_memory.h"
#include "gb_vram_dma.h"
#include "gb_oam_dma.h"

static const u32 g_DMGColorPalette[4] = { 0xFFE3E6C9, 0xFFC3C4A5, 0xFF8E8B61, 0xFF6C6C4E };
static const s32 g_modeclk[4]         = { 204, 456, 80, 172 };

static gb_PaletteMemory g_ColorRAM[2];

static u8  g_VRAM_all[2 * 8192];
static u8 *g_VRAM;
static u8  g_OAM[256];

static u8  g_linebuffer[3][160];
static u32 g_backbuffer[144][160];

static u8  g_LCDC;
static u8  g_STAT;
static u8  g_SCY;
static u8  g_SCX;
static u8  g_LY;
static u8  g_LYC;
static u8  g_BGP[4];
static u8  g_OBP[2][4];
static u8  g_WY;
static u8  g_WX;
static u8  g_VBK;

static bool g_stat_irq_line;
static s32  g_mode;
static s32  g_ticks;

//-----------------------------------------------------------------------------
// Renderer
//-----------------------------------------------------------------------------

static void gb_UnpackPaletteData(const u8 tiledata[2], u8 palettedata[8], bool flipx)
{
    if (flipx) { for (s32 x = 0; x < 8; x++) { palettedata[x] = ((tiledata[0] >> (    x)) & 1) | (((tiledata[1] >> (    x)) << 1) & 2); } }
    else       { for (s32 x = 0; x < 8; x++) { palettedata[x] = ((tiledata[0] >> (7 - x)) & 1) | (((tiledata[1] >> (7 - x)) << 1) & 2); } }
}

static void gb_GetBGWindowTileData(u8 tileid, s32 line, u8 tiledata[2], bool bank)
{
    s32 base = (bank ? 0x2000 : 0x0000) + (BIT_TEST(g_LCDC, 4) ? (tileid * 16) : (0x1000 + ((s8)tileid) * 16)) + (2 * line);
    tiledata[0] = g_VRAM_all[base];
    tiledata[1] = g_VRAM_all[base + 1];
}

static void gb_GetOBJTileData(u8 tileid, s32 line, u8 tiledata[2], bool bank)
{
    s32 base = (bank ? 0x2000 : 0x0000) + (tileid * 16) + (2 * line);
    tiledata[0] = g_VRAM_all[base];
    tiledata[1] = g_VRAM_all[base + 1];
}

static void gb_GetBGWindowTileRow(u8 tileid, s32 line, u8 dots[8], bool bank, bool flipx, u8 cgbpalette, u8 cgbpriority)
{
    u8 tiledata[2];
    gb_GetBGWindowTileData(tileid, line, tiledata, bank);
    gb_UnpackPaletteData(tiledata, dots, flipx);
    for (s32 x = 0; x < 8; x++) { dots[x] = (cgbpriority << 7) | (cgbpalette << 4) | (g_BGP[dots[x]] << 2) | dots[x]; }
}

static void gb_GetOBJTileRow(u8 tileid, s32 line, u8 dots[8], bool bank, bool flipx, u8 dmgpalette, u8 cgbpalette, u8 objpriority)
{
    u8 tiledata[2];
    gb_GetOBJTileData(tileid, line, tiledata, bank);
    gb_UnpackPaletteData(tiledata, dots, flipx);
    for (s32 x = 0; x < 8; x++) { dots[x] = (objpriority << 7) | (cgbpalette << 4) | (g_OBP[dmgpalette][dots[x]] << 2) | dots[x]; }
}

static void gb_ClearBuffer()
{
    memset(g_linebuffer, 0, sizeof(g_linebuffer));
}

static void gb_WriteBGWindowPixelToBuffer(s32 hpos, u8 dot)
{
    if (hpos >= 0 && hpos < 160) { g_linebuffer[0][hpos] = dot; }
}

static void gb_WriteOBJPixelToBuffer(s32 hpos, u8 dot, u8 xpos)
{
    if (!(dot & 3) || (hpos < 0) || (hpos >= 160)) { return; }

    if (g_linebuffer[2][hpos])
    {
        if (gb_IsCGB() || (g_linebuffer[1][hpos] <= xpos)) { return; }
    }
    else
    {
        if (BIT_TEST(g_LCDC, 0) && ((BIT_TEST(dot, 7) && (g_linebuffer[0][hpos] & 3)) || BIT_TEST(g_linebuffer[0][hpos], 7))) { return; }
    }

    g_linebuffer[0][hpos] = dot;
    g_linebuffer[1][hpos] = xpos;
    g_linebuffer[2][hpos] = 0xFF;
}

static void gb_DrawBG()
{
    s32 mapbase = BIT_TEST(g_LCDC, 3) ? 0x1C00 : 0x1800;
    s32 ypos    = ((g_SCY + g_LY) >> 3) & 0x1F;
    s32 bgv     =  (g_SCY + g_LY)       & 7;
    s32 xpos    =  (g_SCX         >> 3) & 0x1F;
    s32 bgh     =   g_SCX               & 7;
    s32 idbasey = mapbase + (ypos * 32);
    u8  fbgv    = 7 - bgv;
    u8  dots[21][8];
    
    for (s32 i = 0; i < 21; i++)
    {
        s32 index      = idbasey + ((xpos + i) & 31);
        u8  attributes = g_VRAM_all[0x2000 + index];
        gb_GetBGWindowTileRow(g_VRAM_all[index], BIT_TEST(attributes, 6) ? fbgv : bgv, dots[i], BIT_TEST(attributes, 3), BIT_TEST(attributes, 5), BIT_FIELD(attributes, 0, 7), BIT_FIELD(attributes, 7, 1));
    }

    for (s32 i = 0; i < 160; i++)
    {
        s32 x = (i + bgh);
        gb_WriteBGWindowPixelToBuffer(i, dots[x >> 3][x & 7]);
    }
}

static void gb_DrawWindow()
{
    if ((g_LY < g_WY) || (g_WY >= 144) || (g_WX >= 167)) { return; }

    s32 mapbase = BIT_TEST(g_LCDC, 6) ? 0x1C00 : 0x1800;
    s32 dy      = g_LY - g_WY;
    s32 ypos    = (dy >> 3) & 0x1F;
    s32 wnv     =  dy       & 7;
    s32 xpos    = g_WX - 7;
    s32 idbasey = mapbase + (ypos * 32);
    s32 fwnv    = 7 - wnv;
    s32 offx    = 160 - xpos;
    u8  dots[21][8];

    for (s32 i = 0; i < ((offx >> 3) + 1); i++)
    {
        s32 index      = idbasey + i;
        u8  attributes = g_VRAM_all[0x2000 + index];
        gb_GetBGWindowTileRow(g_VRAM_all[index], BIT_TEST(attributes, 6) ? fwnv : wnv, dots[i], BIT_TEST(attributes, 3), BIT_TEST(attributes, 5), BIT_FIELD(attributes, 0, 7), BIT_FIELD(attributes, 7, 1));
    }

    for (s32 i = 0; i < offx; i++)
    {
        gb_WriteBGWindowPixelToBuffer(xpos + i, dots[i >> 3][i & 7]);
    }
}

static bool gb_DrawOBJTile(s32 entry)
{
    bool sizeflag = BIT_TEST(g_LCDC, 2);
    s32  size     = sizeflag ? 16 : 8;
    s32  id       = entry * 4;

    s32 ypos = g_OAM[id];
    s32 ty   = ypos - 16;
    if (ypos >= 160 || ypos <= 0 || g_LY >= (ty + size) || g_LY < ty) { return false; }

    s32 xpos = g_OAM[id + 1];
    if (xpos >= 168 || xpos <= 0) { return true; }
    s32 objh = xpos - 8;
    
    u8  attributes = g_OAM[id + 3];
    s32 obv        = g_LY - ty;
    s32 line       = BIT_TEST(attributes, 6) ? ((size - 1) - obv) : obv;

    u8 tileid = g_OAM[id + 2];
    if (sizeflag) { tileid = (tileid & 0xFE) | ((line >> 3) & 1); }

    u8 dots[8];
    gb_GetOBJTileRow(tileid, line & 7, dots, BIT_TEST(attributes, 3) && gb_IsCGB(), BIT_TEST(attributes, 5), BIT_FIELD(attributes, 4, 1), BIT_FIELD(attributes, 0, 7), BIT_FIELD(attributes, 7, 1));
    for (s32 i = 0; i < 8; i++) { gb_WriteOBJPixelToBuffer(objh++, dots[i], xpos); }

    return true;
}

static void gb_DrawOBJ()
{
    s32 objs_selected = 0;
    for (s32 i = 0; (i < 40) && (objs_selected < 10); i++) { objs_selected += gb_DrawOBJTile(i); }
}

static void gb_ClearBackBuffer()
{
    memset(g_backbuffer, 0xFF, sizeof(g_backbuffer));
}

static void gb_RenderLineDMG()
{
    for (s32 i = 0; i < 160; i++) { g_backbuffer[g_LY][i] = g_DMGColorPalette[BIT_FIELD(g_linebuffer[0][i], 2, 3)]; }
}

static void gb_RenderLineCGB()
{
    for (s32 i = 0; i < 160; i++) { g_backbuffer[g_LY][i] = g_ColorRAM[g_linebuffer[2][i] ? 1 : 0].GetColor32(BIT_FIELD(g_linebuffer[0][i], 4, 7), BIT_FIELD(g_linebuffer[0][i], 0, 3)); }
}

static void gb_RenderLine()
{
    bool bgwindow_enable = BIT_TEST(g_LCDC, 0) || gb_IsCGB();

    gb_ClearBuffer();

    if (bgwindow_enable)                        { gb_DrawBG(); }
    if (BIT_TEST(g_LCDC, 5) && bgwindow_enable) { gb_DrawWindow(); }
    if (BIT_TEST(g_LCDC, 1))                    { gb_DrawOBJ(); }

    if (gb_IsCGB()) { gb_RenderLineCGB(); } else { gb_RenderLineDMG(); } 
}

static void gb_RenderFrame()
{
    RenderFrame(g_backbuffer);
}

//-----------------------------------------------------------------------------
// Control
//-----------------------------------------------------------------------------

void gb_ResetLCD()
{
    gb_ResetHDMA();
    gb_ResetDMA();

    g_ColorRAM[0].Reset();
    g_ColorRAM[1].Reset();

    memset(g_VRAM_all, 0, sizeof(g_VRAM_all));
    g_VRAM = &g_VRAM_all[0];
    memset(g_OAM, 0, sizeof(g_OAM));

    gb_ClearBuffer();
    gb_ClearBackBuffer();

    g_LCDC   = 0x91;
    g_STAT   = 0x81;
    g_SCY    = 0;
    g_SCX    = 0;
    g_LY     = 0x91;
    g_LYC    = 0;
    g_BGP[0] = 0;
    g_BGP[1] = 3;
    g_BGP[2] = 3;
    g_BGP[3] = 3;
    memset(g_OBP, 0, sizeof(g_OBP));
    g_WY     = 0;
    g_WX     = 0;
    g_VBK    = 0xFE;

    g_stat_irq_line = false;
    g_mode          = 1;
    g_ticks         = g_modeclk[g_mode];
}

static void gb_CheckSTATInterrupts()
{
    bool irq = (BIT_TEST(g_STAT, 6) && (g_LY == g_LYC)) || (BIT_TEST(g_STAT, 5) && (g_mode == 2)) || (BIT_TEST(g_STAT, 4) && (g_mode == 1)) || (BIT_TEST(g_STAT, 3) && (g_mode == 0));
    if (!g_stat_irq_line && irq) { gb_RequestInterrupt(IRQ_LCD); }
    g_stat_irq_line = irq;
}

static void gb_TurnOffLCD()
{
    // What happens here?
}

static void gb_TurnOnLCD()
{
    // and here?
}

static void gb_LCDEnterVBlank()
{
    g_mode = 1;
    gb_RequestInterrupt(IRQ_VBL);
    gb_RenderFrame();
}

static void gb_LCDLeaveVBlank()
{
    g_LY   = 0;
    g_mode = 2;
}

static void gb_LCDEnterHBlank()
{
    if (g_LY < 144) { gb_RenderLine(); }
    g_mode = 0;
    gb_HBlankTransfer();
}

static void gb_AdvanceLCDSequence()
{
    switch (g_mode)
    {
    case 0: if (++g_LY >= 144) { gb_LCDEnterVBlank(); } else { g_mode = 2; } break;
    case 1: if (++g_LY >= 154) { gb_LCDLeaveVBlank(); }                      break;
    case 2:                                                    g_mode = 3;   break;
    case 3:                      gb_LCDEnterHBlank();                        break;
    }

    gb_CheckSTATInterrupts();
    g_ticks += g_modeclk[g_mode];
}

void gb_SyncLCD(s32 ticks)
{
    if (!BIT_TEST(g_LCDC, 7)) { return; }
    g_ticks -= ticks;
    while (g_ticks <= 0) { gb_AdvanceLCDSequence(); }
}

//-----------------------------------------------------------------------------
// I/O
//-----------------------------------------------------------------------------

void gb_WriteVRAM(u16 offset, u8 byte)
{
    g_VRAM[offset & 0x1FFF] = byte;
}

void gb_WriteOAM(u16 offset, u8 byte)
{
    g_OAM[offset & 0xFF] = byte;
}

u8 gb_ReadVRAM(u16 offset)
{
    return g_VRAM[offset & 0x1FFF];
}

u8 gb_ReadOAM(u16 offset)
{
    return g_OAM[offset & 0xFF];
}

void gb_WriteLCDC(u8 byte)
{
    if (BIT_TEST(g_LCDC ^ byte, 7)) { if (BIT_TEST(byte, 7)) { gb_TurnOnLCD(); } else { gb_TurnOffLCD(); } }
    g_LCDC = byte;
}

void gb_WriteSTAT(u8 byte)
{
    g_STAT = byte & 0xF8;
    gb_CheckSTATInterrupts();
}

void gb_WriteSCY(u8 byte)
{
    g_SCY = byte;
}

void gb_WriteSCX(u8 byte)
{
    g_SCX = byte;
}

void gb_WriteLYC(u8 byte)
{
    g_LYC = byte;
    gb_CheckSTATInterrupts();
}

void gb_WriteBGP(u8 byte)
{
    for (s32 i = 0; i < 4; i++) { g_BGP[i] = (byte >> (i * 2)) & 3; }
}

void gb_WriteOBP0(u8 byte)
{
    for (s32 i = 0; i < 4; i++) { g_OBP[0][i] = (byte >> (i * 2)) & 3; }
}

void gb_WriteOBP1(u8 byte)
{
    for (s32 i = 0; i < 4; i++) { g_OBP[1][i] = (byte >> (i * 2)) & 3; }
}

void gb_WriteWY(u8 byte)
{
    g_WY = byte;
}

void gb_WriteWX(u8 byte)
{
    g_WX = byte;
}

void gb_WriteVBK(u8 byte)
{
    if (!gb_IsCGB()) { return; }
    g_VBK  = byte;
    g_VRAM = g_VRAM_all + (BIT_TEST(byte, 0) ? 0x2000 : 0x0000);
}

void gb_WriteBCPS(u8 byte)
{
    g_ColorRAM[0].WritePS(byte);
}

void gb_WriteBCPD(u8 byte)
{
    g_ColorRAM[0].WritePD(byte);
}

void gb_WriteOCPS(u8 byte)
{
    g_ColorRAM[1].WritePS(byte);
}

void gb_WriteOCPD(u8 byte)
{
    g_ColorRAM[1].WritePD(byte);
}

u8 gb_ReadLCDC()
{
    return g_LCDC;
}

u8 gb_ReadSTAT()
{
    return 0x80 | (g_STAT & 0x78) | ((g_LY == g_LYC) ? 0x04 : 0) | g_mode;
}

u8 gb_ReadSCY()
{
    return g_SCY;
}

u8 gb_ReadSCX()
{
    return g_SCX;
}

u8 gb_ReadLY()
{
    return g_LY;
}

u8 gb_ReadLYC()
{
    return g_LYC;
}

u8 gb_ReadBGP()
{
    return (g_BGP[3] << 6) | (g_BGP[2] << 4) | (g_BGP[1] << 2) | g_BGP[0];
}

u8 gb_ReadOBP0()
{
    return (g_OBP[0][3] << 6) | (g_OBP[0][2] << 4) | (g_OBP[0][1] << 2) | g_OBP[0][0];
}

u8 gb_ReadOBP1()
{
    return (g_OBP[1][3] << 6) | (g_OBP[1][2] << 4) | (g_OBP[1][1] << 2) | g_OBP[1][0];
}

u8 gb_ReadWY()
{
    return g_WY;
}

u8 gb_ReadWX()
{
    return g_WX;
}

u8 gb_ReadVBK()
{
    return gb_IsCGB() ? (g_VBK | 0xFE) : 0xFF;
}

u8 gb_ReadBCPS()
{
    return gb_IsCGB() ? g_ColorRAM[0].ReadPS() : 0xFF;
}

u8 gb_ReadBCPD()
{
    return gb_IsCGB() ? g_ColorRAM[0].ReadPD() : 0xFF;
}

u8 gb_ReadOCPS()
{
    return gb_IsCGB() ? g_ColorRAM[1].ReadPS() : 0xFF;
}

u8 gb_ReadOCPD()
{
    return gb_IsCGB() ? g_ColorRAM[1].ReadPD() : 0xFF;
}
