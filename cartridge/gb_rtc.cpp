//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include "../gb_host.h"

static u8 g_seconds;
static u8 g_minutes;
static u8 g_hours;
static u8 g_days_lo;
static u8 g_control;

void gb_LatchRTC()
{
	u16 d;
	ReadRTC(g_seconds, g_minutes, g_hours, d);
	g_days_lo = d & 0xFF;
	g_control = (g_control & 0xFE) | (BIT_FIELD(d, 8, 1));
}

void gb_ResetRTC()
{
	g_control = 0;
	gb_LatchRTC();
}

void gb_WriteRTC(u8 port, u8 byte)
{
	switch (port)
	{
	case 0x08: g_seconds = byte; break;
	case 0x09: g_minutes = byte; break;
	case 0x0A: g_hours   = byte; break;
	case 0x0B: g_days_lo = byte; break;
	case 0x0C: g_control = byte; break;
	}
}

u8 gb_ReadRTC(u8 port)
{
	switch (port)
	{
	case 0x08: return g_seconds;
	case 0x09: return g_minutes;
	case 0x0A: return g_hours;
	case 0x0B: return g_days_lo;
	case 0x0C: return g_control;
	}

	return 0;
}

/*
void gb_IncrementRTC()
{
	u16 daycounter;
	g_RTCticks += g_RTCclk;
	g_RTCT[0]++;
	if (g_RTCT[0] < 60) {return;}
	g_RTCT[0] = 0;
	g_RTCT[1]++;
	if (g_RTCT[1] < 60) {return;}
	g_RTCT[1] = 0;
	g_RTCT[2]++;
	if (g_RTCT[2] < 24) {return;}
	g_RTCT[2] = 0;
	daycounter = ((g_RTCT[3] | (g_RTCT[4] << 8)) & 0x01FF) + 1;
	g_RTCT[3] = daycounter & 0xFF;
	g_RTCT[4] = (g_RTCT[4] & 0xFE) | ((daycounter >> 8) & 1) | ((daycounter >> 2) & 0x80);
}

void gb_SyncRTC(s32 ticks)
{
	if (!g_hasRTC || g_RTCT[4] & 0x40) {return;}
	g_RTCticks -= ticks;
	while (g_RTCticks <= 0) {gb_IncrementRTC();}
}
*/
