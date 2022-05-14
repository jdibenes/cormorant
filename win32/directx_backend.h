//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#pragma once

bool InitializeDirect3D(HWND hWndMain);
void CleanUpDirect3D();
bool InitializeDirectSound(HWND hWndMain);
void CleanUpDirectSound();
bool InitializeDirectInput(HINSTANCE hInst, HWND hWndMain);
void CleanUpDirectInput();
