//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include "../gb_host.h"

#define KEYDOWN(name, key) ((name[key] & 0x80) ? true : false);

static LPDIRECTINPUT8       pInput;
static LPDIRECTINPUTDEVICE8 pPad;
static u8                   KeyBuffer[256];

bool InitializeDirectInput(HINSTANCE hInst, HWND hWndMain)
{
    HRESULT hrval;

    hrval = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&pInput, NULL);
    if (FAILED(hrval)) {return false;}

    hrval = pInput->CreateDevice(GUID_SysKeyboard, &pPad, NULL);
    if (FAILED(hrval)) {return false;}

    hrval = pPad->SetDataFormat(&c_dfDIKeyboard);
    if (FAILED(hrval)) {return false;}

    hrval = pPad->SetCooperativeLevel(hWndMain, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
    if (FAILED(hrval)) {return false;}

    pPad->Acquire();
    
    return true;
}

void ReadJoypad(bool &a, bool &b, bool &select, bool &start, bool &right, bool &left, bool &up, bool &down)
{
    memset(KeyBuffer, 0, sizeof(KeyBuffer));

    pPad->GetDeviceState(sizeof(KeyBuffer), KeyBuffer);

    a      = KEYDOWN(KeyBuffer, DIK_X);
    b      = KEYDOWN(KeyBuffer, DIK_C);
    select = KEYDOWN(KeyBuffer, DIK_SPACE);
    start  = KEYDOWN(KeyBuffer, DIK_RETURN);
    right  = KEYDOWN(KeyBuffer, DIK_RIGHT);
    left   = KEYDOWN(KeyBuffer, DIK_LEFT);
    up     = KEYDOWN(KeyBuffer, DIK_UP);
    down   = KEYDOWN(KeyBuffer, DIK_DOWN);
}

void CleanUpDirectInput()
{
    if (pPad)   { pPad->Unacquire(); pPad->Release();   pPad   = NULL; }
    if (pInput) {                    pInput->Release(); pInput = NULL; }
}
