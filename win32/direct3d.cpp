//-------------------------------------------------------------------------------------------------
// Project Cormorant - GB / CGB Emulator
// jcds (jdibenes@outlook.com)
// 2011
//-------------------------------------------------------------------------------------------------

#include <d3d9.h>
#include "../gb_host.h"

#define D3DFVF_TLVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define QUAD_L          256

struct D3DTLVERTEX
{
    float x;
    float y;
    float z;
    float rhw;
    D3DCOLOR color;
    float u;
    float v;
};

static IDirect3D9             *pD3D9;
static IDirect3DDevice9       *pDevice;
static IDirect3DVertexBuffer9 *pVertexBuffer;
static IDirect3DTexture9      *pTexture;
static D3DPRESENT_PARAMETERS   D3Dpp;
static D3DDISPLAYMODE          DisplayMode;

static bool InitializeDevice(HWND hWndMain)
{
    HRESULT hrval;

    ZeroMemory(&D3Dpp, sizeof(D3Dpp));

    D3Dpp.SwapEffect           = D3DSWAPEFFECT_DISCARD;
    D3Dpp.BackBufferCount      = 1;
    D3Dpp.BackBufferWidth      = GB_WIDTH;
    D3Dpp.BackBufferHeight     = GB_HEIGHT;
    D3Dpp.BackBufferFormat     = DisplayMode.Format;
    D3Dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    D3Dpp.Windowed             = TRUE;

    hrval = pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWndMain, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &D3Dpp, &pDevice);
    if (hrval == D3D_OK) {return true;}

    hrval = pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWndMain, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &D3Dpp, &pDevice);
    if (hrval == D3D_OK) {return true;}

    hrval = pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWndMain, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &D3Dpp, &pDevice);

    return hrval == D3D_OK;
}

static bool InitializeVertexBuffer()
{
    D3DTLVERTEX *vertices;
    HRESULT hrval;

    pDevice->SetFVF(D3DFVF_TLVERTEX);

    hrval = pDevice->CreateVertexBuffer(sizeof(D3DTLVERTEX) * 4, 0, D3DFVF_TLVERTEX, D3DPOOL_DEFAULT, &pVertexBuffer, NULL);
    if (hrval != D3D_OK) {return false;}

    pVertexBuffer->Lock(0, 0, (void **)&vertices, NULL);

    vertices[0].x =   0.0f; vertices[0].y = QUAD_L; vertices[0].z = 0.0f; vertices[0].rhw = 1.0f; vertices[0].color = 0xFFFFFFFF; vertices[0].u = 0.0f; vertices[0].v = 1.0f;
    vertices[1].x =   0.0f; vertices[1].y =   0.0f; vertices[1].z = 0.0f; vertices[1].rhw = 1.0f; vertices[1].color = 0xFFFFFFFF; vertices[1].u = 0.0f; vertices[1].v = 0.0f;
    vertices[2].x = QUAD_L; vertices[2].y = QUAD_L; vertices[2].z = 0.0f; vertices[2].rhw = 1.0f; vertices[2].color = 0xFFFFFFFF; vertices[2].u = 1.0f; vertices[2].v = 1.0f;
    vertices[3].x = QUAD_L; vertices[3].y =   0.0f; vertices[3].z = 0.0f; vertices[3].rhw = 1.0f; vertices[3].color = 0xFFFFFFFF; vertices[3].u = 1.0f; vertices[3].v = 0.0f;

    pVertexBuffer->Unlock();

    return true;
}

static bool InitializeTexture()
{
    HRESULT hrval;
    hrval = pDevice->CreateTexture(QUAD_L, QUAD_L, 0, D3DUSAGE_DYNAMIC, DisplayMode.Format, D3DPOOL_DEFAULT, &pTexture, NULL);
    return hrval == D3D_OK;
}

bool InitializeDirect3D(HWND hWndMain)
{
    pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
    if (!pD3D9) {return false;}

    pD3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &DisplayMode);

    if (!InitializeDevice(hWndMain))       {return false;}
    if (!InitializeVertexBuffer()) {return false;}
    if (!InitializeTexture())      {return false;}

    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    pDevice->SetStreamSource(0, pVertexBuffer, 0, sizeof(D3DTLVERTEX));
    pDevice->SetTexture(0, pTexture);

    return true;
}

void RenderFrame(u32 buf[GB_HEIGHT][GB_WIDTH])
{
    D3DLOCKED_RECT locked_rect;

    pTexture->LockRect(0, &locked_rect, NULL, 0);

    for (s32 line  = 0; line  < GB_HEIGHT; line++)
    {
    for (s32 index = 0; index < GB_WIDTH;  index++)
    {
        ((u32*)locked_rect.pBits)[(line * QUAD_L) + index] = buf[line][index];
    }
    }

    pTexture->UnlockRect(0);

    pDevice->BeginScene();
    pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
    pDevice->EndScene();

    pDevice->Present(NULL, NULL, NULL, NULL);
}

void CleanUpDirect3D()
{
    if (pTexture)      { pDevice->SetTexture(0, NULL);            pTexture->Release();      pTexture      = NULL; }
    if (pVertexBuffer) { pDevice->SetStreamSource(0, NULL, 0, 0); pVertexBuffer->Release(); pVertexBuffer = NULL; }
    if (pDevice)       {                                          pDevice->Release();       pDevice       = NULL; }
    if (pD3D9)         {                                          pD3D9->Release();         pD3D9         = NULL; }
}
