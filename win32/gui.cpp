//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include <Windows.h>
#include <stdio.h>
#include "directx_backend.h"
#include "../gb_guest.h"
#include "../gb_host.h"
#include "resource.h"
#include "loader.h"

#define MAINWND_CLASS "gbc38x_wcls0"
#define MAINWND_TITLE "Cormorant v0.4"

enum MENU_ENUM
{
    MENU_FILE = 0x1000,
    FILE_OPENROM,
    FILE_EXIT
};

static HINSTANCE hInst;
static HWND      hWndMain;
static HANDLE    hConsoleOut = INVALID_HANDLE_VALUE;
static HANDLE    hCoreThread;
static DWORD     dwCoreThreadId;
static HMENU     hMenuMain;
static POINT     MinSize;
static char      FileName[MAX_PATH];

static void Panic(char const* message)
{
    MessageBox(hWndMain, message, "Error", MB_ICONHAND | MB_OK);
    exit(0);
}

void ShowMessage(const char* format, ...)
{
    char* text;
    int len;
    va_list arg_list;
    DWORD bw;
    if (hConsoleOut == INVALID_HANDLE_VALUE) { return; }
    va_start(arg_list, format);
    len = _vscprintf(format, arg_list) + 1;
    text = (char*)malloc(len);
    if (!text) { return; }
    vsprintf_s(text, len, format, arg_list);
    va_end(arg_list);
    text[len - 1] = '\n';
    WriteFile(hConsoleOut, text, len, &bw, NULL);
    free(text);
}

static void AdjustMainClientArea(int x, int y)
{
    int cx, cy;
    RECT rWindow, rClient;
    GetWindowRect(hWndMain, &rWindow);
    GetClientRect(hWndMain, &rClient);
    cx = ((rWindow.right - rWindow.left) - rClient.right) + x;
    cy = ((rWindow.bottom - rWindow.top) - rClient.bottom) + y;
    SetWindowPos(hWndMain, NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
}

static void CloseEmulatorCoreThread()
{
    DWORD ecode;
    if (!hCoreThread) { return; }
    ShowMessage("Esperando que el hilo de ejecucion del emulador termine...");
    gb_StopEmulation();
    WaitForSingleObject(hCoreThread, INFINITE);
    GetExitCodeThread(hCoreThread, &ecode);
    ShowMessage("El hilo de ejecucion del emulador (0x%X) ha finalizado con codigo %d (0x%X)", dwCoreThreadId, ecode, ecode);
    CloseHandle(hCoreThread);
    hCoreThread = NULL;
}

static bool LoadFile()
{
    bool ret = FileLoader(FileName);
    if (ret) { return true; }        
    ShowMessage("Error al leer el archivo %", FileName);
    return false;
}

static bool CreateCoreThread()
{
    hCoreThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)gb_StartEmulation, NULL, CREATE_SUSPENDED, &dwCoreThreadId);
    if (hCoreThread) { return true; }
    ShowMessage("Error al crear el hilo de ejecucion del emulador");
    return false;
}

static void StartEmulation()
{
    CloseEmulatorCoreThread();
    ShowMessage(" ");
    ShowMessage("===============================================================================");
    ShowMessage(" ");
    if (!LoadFile()) { return; }
    if (!CreateCoreThread()) { return; }
    ShowMessage("Se ha creado el hilo de ejecucion del emulador con id: 0x%X", dwCoreThreadId);
    ResumeThread(hCoreThread);
}

static int SelectFileToLoad()
{
    OPENFILENAME openfilename;
    memset(FileName, 0, sizeof(FileName));
    memset(&openfilename, 0, sizeof(OPENFILENAME));
    openfilename.lStructSize = sizeof(OPENFILENAME);
    openfilename.hwndOwner = hWndMain;
    openfilename.lpstrFilter = "ROMs de Game Boy (*.gb;*.gbc;*.cgb)\0*.gb;*.gbc;*.cgb\0Todos los archivos (*.*)\0*.*\0";
    openfilename.lpstrFile = FileName;
    openfilename.nMaxFile = sizeof(FileName);
    openfilename.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    return GetOpenFileName(&openfilename);
}

static void CloseOutputConsole()
{
    if (hConsoleOut == INVALID_HANDLE_VALUE) { return; }
    CloseHandle(hConsoleOut);
    hConsoleOut = INVALID_HANDLE_VALUE;
}

static void MainCleanUp()
{
    CloseEmulatorCoreThread();
    FreePakMemory();

    CleanUpDirect3D();
    CleanUpDirectSound();    
    CleanUpDirectInput();
    CloseOutputConsole();
}

static LRESULT CALLBACK MainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    switch (uMsg)
    {
    case WM_PAINT:
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case FILE_OPENROM: if (SelectFileToLoad()) { StartEmulation(); } break;
        case FILE_EXIT:    DestroyWindow(hWnd);                          break;
        }
        break;
    case WM_GETMINMAXINFO:        
        ((MINMAXINFO *)lParam)->ptMinTrackSize.x = MinSize.x;
        ((MINMAXINFO *)lParam)->ptMinTrackSize.y = MinSize.y;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

static BOOL WINAPI ConsoleProc(DWORD dwCtrlType)
{
    switch (dwCtrlType)
    {
    case CTRL_CLOSE_EVENT:
        MainCleanUp();
        return TRUE;
    default:
        return FALSE;
    }
}

static void InitOutputConsole()
{
    if (!AllocConsole()) { return; }
    SendMessage(GetConsoleWindow(), WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(MAIN_ICON)));
    hConsoleOut = CreateFile("CONOUT$", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hConsoleOut == INVALID_HANDLE_VALUE) { return; }
    SetConsoleTitle(MAINWND_TITLE);
    SetConsoleCtrlHandler(ConsoleProc, TRUE);
}

static void CreateMainWindowMenu()
{
    MENUITEMINFO minfo;
    HMENU hMenuSub;
    memset(&minfo, 0, sizeof(minfo));
    hMenuMain = CreateMenu();
    hMenuSub = CreateMenu();
    minfo.cbSize = sizeof(MENUITEMINFO);
    minfo.fMask = MIIM_ID | MIIM_STATE | MIIM_TYPE;
    minfo.wID = FILE_OPENROM;
    minfo.fState = MFS_ENABLED;
    minfo.fType = MFT_STRING;
    minfo.dwTypeData = (LPSTR)"Abrir &ROM";
    InsertMenuItem(hMenuSub, 0, TRUE, &minfo);
    minfo.fMask = MIIM_TYPE;
    minfo.fType = MFT_SEPARATOR;
    InsertMenuItem(hMenuSub, 1, TRUE, &minfo);
    minfo.fMask = MIIM_ID | MIIM_STATE | MIIM_TYPE;
    minfo.wID = FILE_EXIT;
    minfo.fType = MFT_STRING;
    minfo.dwTypeData = (LPSTR)"&Salir\tAlt + F4";
    InsertMenuItem(hMenuSub, 2, TRUE, &minfo);
    minfo.fMask = MIIM_ID | MIIM_STATE | MIIM_TYPE | MIIM_SUBMENU;
    minfo.wID = MENU_FILE;
    minfo.dwTypeData = (LPSTR)"&Archivo";
    minfo.hSubMenu = hMenuSub;
    InsertMenuItem(hMenuMain, 0, TRUE, &minfo);
}

static int RegisterWindowClass()
{
    WNDCLASS wndclass;
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndclass.lpfnWndProc = MainProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInst;
    wndclass.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(MAIN_ICON));
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = MAINWND_CLASS;
    return RegisterClass(&wndclass);
}

static void CalculateMainWindowMinSize()
{
    RECT windowrc;
    AdjustMainClientArea(GB_WIDTH, GB_HEIGHT);
    GetWindowRect(hWndMain, &windowrc);
    MinSize.x = windowrc.right - windowrc.left;
    MinSize.y = windowrc.bottom - windowrc.top;
}

static void LoadCmdLineFile(LPSTR lpCmdLine)
{
    lpCmdLine[strlen(lpCmdLine) - 1] = '\0';
    strcpy_s(FileName, sizeof(FileName), &lpCmdLine[1]);
    StartEmulation();
}

static void PrintLogo()
{
    ShowMessage("===============================================================================");
    ShowMessage("Cormorant - Emulador de Game Boy Color");
    ShowMessage("jcds");
    ShowMessage("Version 0.4");
    ShowMessage("2011");
    ShowMessage("===============================================================================");
    ShowMessage(" ");
    ShowMessage("Seleccione la opcion \"Abrir ROM\" del menu \"Archivo\" para cargar un programa en el emulador");
    ShowMessage("Controles: X = A, C = B, ESPACIO = SELECT, ENTER = START, FLECHAS = D-PAD");
}

static void InitApplication()
{    
    InitOutputConsole();

    if (!InitializeDirect3D(hWndMain))           { Panic("Error al inicializar Direct3D");    }
    if (!InitializeDirectSound(hWndMain))        { Panic("Error al inicializar DirectSound"); }
    if (!InitializeDirectInput(hInst, hWndMain)) { Panic("Error al inicializar DirectInput"); }

    gb_Initialize();

    PrintLogo();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int)
{
    hInst = hInstance;
    if (!RegisterWindowClass()) { return 0; }

    MinSize.x = GB_WIDTH;
    MinSize.y = GB_HEIGHT;
    CreateMainWindowMenu();

    hWndMain = CreateWindowEx(0, MAINWND_CLASS, MAINWND_TITLE, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, NULL, hMenuMain, hInstance, NULL);
    if (!hWndMain) { return 0; }

    InitApplication();

    CalculateMainWindowMinSize();
    ShowWindow(hWndMain, SW_SHOWDEFAULT);
    UpdateWindow(hWndMain);

    if (strlen(lpCmdLine) > 0) { LoadCmdLineFile(lpCmdLine); }

    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    MainCleanUp();
    return (int)msg.wParam;
}

void ReadRTC(u8 &s, u8 &m, u8 &h, u16 &d)
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    s = st.wSecond    & 0xFF;
    m = st.wMinute    & 0xFF;
    h = st.wHour      & 0xFF;
    d = st.wDayOfWeek & 0x1FF;
}
