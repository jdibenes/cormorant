//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#define _CRT_SECURE_NO_WARNINGS 

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "../gb_host.h"
#include "../cartridge/gb_pak.h"

static char *g_sav_filename = NULL;

static void FreeROM()
{
    u8* rom = gb_GetPakROMAddress();
    if (!rom) { return; }
    free(rom);
    gb_SetPakROMAddress(NULL);
}

static void FreeSRAM()
{
    u8 *sram = gb_GetPakSRAMAddress();
    if (!sram) { return; }
    free(sram);
    gb_SetPakSRAMAddress(NULL);
}

static void FreeSaveFilename()
{
    if (!g_sav_filename) { return; }
    free(g_sav_filename);
    g_sav_filename = NULL;
}

void FreePakMemory() {
    FreeROM();
    FreeSRAM();
    FreeSaveFilename();
}

static bool CompareHeaderChecksum(gb_ROMHeader const *header)
{
    u8 header_checksum = header->headerchecksum;
    u8 sum = gb_ComputeHeaderChecksum(header);
    bool ret = sum == header_checksum;
    ShowMessage("Header checksum: %s (0x%X / 0x%X)", ret ? "OK" : "BAD", sum, header_checksum);
    return ret;
}

static void PrintPakFeatures(gb_ROMHeader const *romheader)
{
    char title[17];

    memcpy(title, romheader->title, 15);
    title[15] = romheader->CGBFlag;
    title[gb_IsCGB() ? 11 : 16] = '\0';
    ShowMessage("Titulo: %s", title);

    switch (gb_GetPakMBC())
    {
    case MBC_NONE: ShowMessage("Chip: ROM  (0x%X)", romheader->paktype); break;
    case MBC_1:    ShowMessage("Chip: MBC1 (0x%X)", romheader->paktype); break;
    case MBC_2:    ShowMessage("Chip: MBC2 (0x%X)", romheader->paktype); break;
    case MBC_3:    ShowMessage("Chip: MBC3 (0x%X)", romheader->paktype); break;
    case MBC_5:    ShowMessage("Chip: MBC5 (0x%X)", romheader->paktype); break;
    default:       ShowMessage("Chip: N/A  (0x%X)", romheader->paktype); 
    }

    ShowMessage("SRAM: %s",    gb_GetPakSRAM()    ? "Si" : "No");
    ShowMessage("Battery: %s", gb_GetPakBattery() ? "Si" : "No");
    ShowMessage("RTC: %s",     gb_GetPakRTC()     ? "Si" : "No");
    ShowMessage("Rumble: %s",  gb_GetPakRumble()  ? "Si" : "No");

    if (romheader->ROMsize  > 8) { ShowMessage("Longitud de ROM: desconocido (0x%X)",  romheader->ROMsize);  } else { ShowMessage("Longitud de ROM: %d bytes (0x%X)",  gb_GetPakROMSize(),  romheader->ROMsize);  }
    if (romheader->SRAMsize > 5) { ShowMessage("Longitud de SRAM: desconocido (0x%X)", romheader->SRAMsize); } else { ShowMessage("Longitud de SRAM: %d bytes (0x%X)", gb_GetPakSRAMSize(), romheader->SRAMsize); }

    ShowMessage("Sistema: %s (0x%X)", gb_IsCGB() ? "CGB" : "DMG", romheader->CGBFlag);
}

static bool LoadROM(FILE* romfile)
{
    gb_ROMHeader header;
    if (fseek(romfile, 0x0104, SEEK_SET) != 0) { return false; }
    if (fread(&header, 1, sizeof(gb_ROMHeader), romfile) != sizeof(gb_ROMHeader)) { return false; }
    if (!CompareHeaderChecksum(&header)) { return false; }
    gb_SetPakFeatures(&header);
    if (gb_ValidatePak() != 0) { return false; }
    PrintPakFeatures(&header);
    u32 romsize = gb_GetPakROMSize();
    u8* rom = (u8*)calloc(romsize, 1);
    if (!rom) { return false; }
    if (fseek(romfile, 0, SEEK_SET) != 0) { return false; }
    if (fread(rom, 1, romsize, romfile) != romsize) { return false; }
    gb_SetPakROMAddress(rom);
    return true;
}

static bool AllocateSRAM()
{
    u32 sramsize = gb_GetPakSRAMSize();
    u32 bytes = (sramsize <= 8192) ? 8192 : sramsize;
    u8 *sram = (u8*)calloc(bytes, 1);
    if (!sram) { return false; }
    gb_SetPakSRAMAddress(sram);
    ShowMessage("Reservados %d bytes para SRAM", bytes);
    return true;
}

static bool GetSaveFilename(char const* filename)
{
    size_t len = strlen(filename);
    g_sav_filename = (char*)calloc(len + 1 + 4, 1);
    if (!g_sav_filename) { return false; }
    strcpy(g_sav_filename, filename);
    char* extset = strrchr(g_sav_filename, '.');
    if (!extset) { extset = strchr(g_sav_filename, '\0'); }
    strcpy(extset, ".sav");
    ShowMessage("Archivo de SRAM: %s", g_sav_filename);
    return true;
}

static bool LoadSRAM()
{
    u32 sramsize = gb_GetPakSRAMSize();
    if (!gb_GetPakBattery() || (sramsize <= 0)) { return true; }
    ShowMessage("Abriendo archivo %s", g_sav_filename);
    FILE* savfile = fopen(g_sav_filename, "rb");
    if (!savfile) { return false; }
    bool ret = (fseek(savfile, 0, SEEK_SET) == 0) && (fread(gb_GetPakSRAMAddress(), 1, sramsize, savfile) == sramsize);
    fclose(savfile);
    if (ret) { ShowMessage("Leidos %d bytes de %s", sramsize, g_sav_filename); }
    return ret;
}

bool SaveBattery() {
    u32 sramsize = gb_GetPakSRAMSize();
    if ((sramsize <= 0) || !gb_GetPakBattery()) { return true; }
    ShowMessage("Guardando archivo %s", g_sav_filename);
    FILE* savfile = fopen(g_sav_filename, "wb");
    if (!savfile) { return false; }
    bool ret = (fseek(savfile, 0, SEEK_SET) == 0) && (fwrite(gb_GetPakSRAMAddress(), 1, sramsize, savfile) == sramsize);
    fclose(savfile);
    if (ret) { ShowMessage("Escritos %d bytes a %s", sramsize, g_sav_filename); }
    return ret;
}

bool FileLoader(char* filename)
{
    FreePakMemory();

    ShowMessage("Abriendo archivo %s", filename);
    FILE* romfile = fopen(filename, "rb");
    if (!romfile) { return false; }
    bool romok = LoadROM(romfile);
    fclose(romfile);
    if (!romok) { return false; }
    ShowMessage("Leidos %d bytes de %s", gb_GetPakROMSize(), filename);

    if (!AllocateSRAM()) { return false; }    
    if (!GetSaveFilename(filename) || !LoadSRAM()) { ShowMessage("Error al leer el archivo de SRAM"); }
    return true;
}
