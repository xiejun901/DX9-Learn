#pragma once
#include "MainWindow.h"
class TestWindow :
    public D3DMainWindow
{
public:
    TestWindow() :D3DMainWindow() {}
    bool Setup() override
    {
        HRESULT hr;
        d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
        if (nullptr == d3d9)
        {
            MessageBox(0, L"Direct3DCreate9() - FAILED", 0, 0);
            return false;
        }
        D3DDISPLAYMODE d3ddm;

        if (FAILED(d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
        {
            // TO DO: Respond to failure of GetAdapterDisplayMode
            return false;
        }
        D3DCAPS9 caps;
        d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
        auto vp = 0;
        if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
            vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
        else
            vp |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

        D3DPRESENT_PARAMETERS d3dpp;
        memset(&d3dpp, 0, sizeof(d3dpp));
        d3dpp.BackBufferWidth = 800;
        d3dpp.BackBufferHeight = 600;
        d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
        d3dpp.BackBufferCount = 1;
        d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
        d3dpp.MultiSampleQuality = 0;
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow = hWnd;
        d3dpp.Windowed = true;
        d3dpp.EnableAutoDepthStencil = true;
        d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
        d3dpp.Flags = 0;
        d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
        d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
        hr = d3d9->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            hWnd,
            vp,
            &d3dpp,
            &device
            );
        if (FAILED(hr))
        {
            d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

            hr = d3d9->CreateDevice(
                D3DADAPTER_DEFAULT,
                D3DDEVTYPE_HAL,
                hWnd,
                vp,
                &d3dpp,
                &device);

            if (FAILED(hr))
            {
                d3d9->Release(); // done with d3d9 object
                ::MessageBox(0, L"CreateDevice() - FAILED", 0, 0);
                return false;
            }
        }
        //d3d9->Release();
        return true;
    }
    void Cleanup() override
    {

    }
    bool Display(float timeDelta) override
    {
        if (device)
        {
            device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE(0.0f,1.0f,0.0f,1.0f), 1.0f, 0);
            device->BeginScene();
            device->EndScene();
            device->Present(0, 0, 0, 0);
        }
        return true;
    }
    IDirect3DDevice9 * device=nullptr;
    IDirect3D9 * d3d9 = nullptr;
};

