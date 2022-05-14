//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "../cartridge/gb_pak.h"
#include "../comms/gb_ir.h"
#include "../comms/gb_serial.h"
#include "../display/gb_display.h"
#include "../pad/gb_pad.h"
#include "../sound/gb_sound.h"
#include "../timers/gb_timers.h"
#include "../gb_host.h"
#include "gb_speed.h"
#include "gb_irq.h"
#include "gb_memory.h"
#include "gb_cpu.h"

static bool g_enablecore;

void gb_Initialize()
{
    gb_BuildInterpreter();
}

static void gb_SyncTimed(s32 cputicks, s32 hdmaticks)
{
    u8  shift        = gb_CGBSpeed() ? 1 : 0;
    s32 ticks_normal = (cputicks >> shift) + hdmaticks;
    s32 ticks_double = cputicks + (hdmaticks << shift);

    gb_SyncTimers(ticks_double);
    gb_SyncSerial(ticks_double);
    gb_SyncLCD(ticks_normal);
    gb_SyncSound(ticks_normal);
    SyncHost(ticks_normal);
}

static void gb_SyncSystem(s32 cputicks)
{
    gb_SyncTimed(cputicks, gb_SyncHDMA());
    s32 hdmaticks = gb_SyncHDMA();
    if (hdmaticks > 0) { gb_SyncTimed(0, hdmaticks); }
    gb_SyncJoypad();
    gb_CheckInterrupts();
}

int gb_StartEmulation()
{
    ShowMessage("Emulacion comenzada");

    gb_ResetLCD();
    gb_ResetSound();
    gb_ResetTimers();
    gb_ResetIR();
    gb_ResetSerial();
    gb_ResetJoypad();
    gb_ResetMemory();
    gb_ResetIRQ();
    gb_ResetSpeed();
    gb_ResetCPU();

    g_enablecore = true;

    OnBeginEmulation();
    while (g_enablecore) { gb_SyncSystem(gb_isHalted() ? 4 : gb_ExecuteNextInstruction()); }

    StopSound();
    SaveBattery();

    ShowMessage("Emulacion terminada");
    return 0;
}

void gb_StopEmulation()
{
    ShowMessage("Finalizando emulacion...");
    g_enablecore = false;
}

void gb_UnknownOpcodeHandler(bool CB, u8 opcode, u16 PC, u16 AF, u16 BC, u16 DE, u16 HL, u16 SP, bool IME)
{
    ShowMessage("Codigo de operacion desconocido 0x%s%02X en PC:0x%04X", CB ? "CB" : "", opcode, PC);
    ShowMessage("AF:0x%04X BC:0x%04X DE:0x%04X HL:0x%04X SP:0x%04X IME:%X", AF, BC, DE, HL, SP, IME);
    gb_StopEmulation();
}
