//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#define DIRECTSOUND_VERSION 0x0800

#include <dsound.h>
#include "../gb_host.h"
#include "../gb_guest.h"

#define BUFFER_SIZE 32768
#define PERIOD      64

static LPDIRECTSOUND8       pDS8;
static LPDIRECTSOUNDBUFFER  pDSB_SO;
static LPDIRECTSOUNDBUFFER8 pSB8_SO8;
static s16                  SoundBuffer[BUFFER_SIZE];
static s32                  BufferPos;

bool InitializeDirectSound(HWND hWndMain)
{
    gb_SetSoundPeriod(PERIOD);

    DWORD samples_per_second = GB_CLK / PERIOD;

    WAVEFORMATEX wfex;
    DSBUFFERDESC dsbd;
    
    if (DirectSoundCreate8(&DSDEVID_DefaultPlayback, &pDS8, NULL) != DS_OK) { return false; }
    if (pDS8->SetCooperativeLevel(hWndMain, DSSCL_PRIORITY)       != DS_OK) { return false; }

    wfex.wFormatTag      = WAVE_FORMAT_PCM;
    wfex.nChannels       = 2;
    wfex.nSamplesPerSec  = samples_per_second;
    wfex.nAvgBytesPerSec = samples_per_second * 4;
    wfex.nBlockAlign     = 4;
    wfex.wBitsPerSample  = 16;
    wfex.cbSize          = 0;

    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = DSBCAPS_GLOBALFOCUS;
    dsbd.dwBufferBytes   = sizeof(SoundBuffer);
    dsbd.dwReserved      = 0;
    dsbd.lpwfxFormat     = &wfex;
    dsbd.guid3DAlgorithm = DS3DALG_DEFAULT;

    if (pDS8->CreateSoundBuffer(&dsbd, &pDSB_SO, NULL)                       != DS_OK) { return false; }
    if (pDSB_SO->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&pSB8_SO8) != DS_OK) { return false; }

    return true;
}

void SendSoundBufferSample(s32 so1, s32 so2)
{
    BYTE *p_data1;
    BYTE *p_data2;
    DWORD p_size1;
    DWORD p_size2;

    SoundBuffer[BufferPos + 1] = (so1 * 64) & 0xFFFF;
    SoundBuffer[BufferPos]     = (so2 * 64) & 0xFFFF;

    BufferPos += 2;
    if (BufferPos < BUFFER_SIZE) { return; }
    BufferPos = 0;

    pSB8_SO8->Stop();
    pSB8_SO8->Lock(0, sizeof(SoundBuffer), (LPVOID*)&p_data1, &p_size1, (LPVOID*)&p_data2, &p_size2, DSBLOCK_ENTIREBUFFER);
    memcpy(p_data1, SoundBuffer, sizeof(SoundBuffer));
    pSB8_SO8->Unlock((LPVOID)p_data1, p_size1, (LPVOID)p_data2, p_size2);
    pSB8_SO8->SetCurrentPosition(0);
    pSB8_SO8->Play(0, 0, 0);
}

void StopSound()
{
    pSB8_SO8->Stop();
}

void ResetSoundBuffer()
{
    memset(SoundBuffer, 0, sizeof(SoundBuffer));
    BufferPos = 0;
}

void CleanUpDirectSound()
{
    if (pSB8_SO8) { pSB8_SO8->Stop(); pSB8_SO8->Release(); pSB8_SO8 = NULL; }
    if (pDSB_SO)  {                   pDSB_SO->Release();  pDSB_SO  = NULL; }
    if (pDS8)     {                   pDS8->Release();     pDS8     = NULL; }
}
