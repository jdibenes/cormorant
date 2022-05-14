//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

#include "types.h"

#define GB_CLK          4194304
#define GB_WIDTH        160
#define GB_HEIGHT       144

void ShowMessage(const char* format, ...);
void RenderFrame(u32 buf[GB_HEIGHT][GB_WIDTH]);
void ResetSoundBuffer();
void SendSoundBufferSample(s32 so1, s32 so2);
void StopSound();
void ReadJoypad(bool &a, bool &b, bool &select, bool &start, bool &right, bool &left, bool &up, bool &down);
void ReadRTC(u8 &s, u8 &m, u8 &h, u16 &d);
bool SaveBattery();
void OnBeginEmulation();
void SyncHost(s32 ticks);
