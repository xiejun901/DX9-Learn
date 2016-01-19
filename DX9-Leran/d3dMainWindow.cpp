#include "d3dMainWindow.h"
#include "d3dUtility.h"
void D3DMainWindow::Init()
{
    if (!InitWindow())
    {
        MessageBox(0, L"INIT Window faield.", 0, 0);
        exit(0);
    }
    if (!InitD3D())
    {
        MessageBox(0, L"INIT D3D faield.", 0, 0);
        exit(0);
    }
}

bool D3DMainWindow::InitWindow()
{
    WNDCLASSEX winClass;

    winClass.lpszClassName = name;
    winClass.cbSize = sizeof(WNDCLASSEX);
    winClass.style = CS_HREDRAW | CS_VREDRAW;
    winClass.lpfnWndProc = D3DMainWindow::MsgProc;
    winClass.hInstance = GetModuleHandle(NULL);
    winClass.hIcon = LoadIcon(GetModuleHandle(NULL), IDI_APPLICATION);
    winClass.hIconSm = LoadIcon(GetModuleHandle(NULL), IDI_APPLICATION);
    winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    winClass.lpszMenuName = NULL;
    winClass.cbClsExtra = 0;
    winClass.cbWndExtra = 0;

    if (!RegisterClassEx(&winClass))
    {
        MessageBox(0, L"RegisterClassEx faield.",0 , 0);
        return false;
    }

    hWnd = CreateWindowEx(NULL, name,
        name,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        0, 0, width, height, NULL, NULL, GetModuleHandle(NULL), NULL);

    if (hWnd == NULL)
    {
        MessageBox(0, L"CrateWindowEx faield.", 0, 0);
        return false;
    }

    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);
    return true;
}

bool D3DMainWindow::InitD3D() 
{ 
    pD3D = Direct3DCreate9(D3D_SDK_VERSION);

    if (pD3D == NULL)
    {
        MessageBox(0, L"Direct3DCreate9 faield.", 0, 0);
        return false;
    }

    D3DDISPLAYMODE d3ddm;

    if (FAILED(pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
    {
        MessageBox(0, L"GetAdapterDisplayMode faield.", 0, 0);
        return false;
    }

    HRESULT hr;

    if (FAILED(hr = pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
        d3ddm.Format, D3DUSAGE_DEPTHSTENCIL,
        D3DRTYPE_SURFACE, D3DFMT_D16)))
    {
        if (hr == D3DERR_NOTAVAILABLE)
            MessageBox(0, L"CheckDeviceFormat faield.", 0, 0);
            return false;
    }

    //
    // Do we support hardware vertex processing? if so, use it. 
    // If not, downgrade to software.
    //

    D3DCAPS9 d3dCaps;

    if (FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL, &d3dCaps)))
    {
        MessageBox(0, L"GetDeviceCaps faield.", 0, 0);
        return false;
    }

    DWORD dwBehaviorFlags = 0;

    if (d3dCaps.VertexProcessingCaps != 0)
        dwBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
    else
        dwBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

    //
    // Everything checks out - create a simple, windowed device.
    //

    D3DPRESENT_PARAMETERS d3dpp;
    memset(&d3dpp, 0, sizeof(d3dpp));

    d3dpp.BackBufferFormat = d3ddm.Format;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.Windowed = TRUE;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
        dwBehaviorFlags, &d3dpp, &pd3dDevice)))
    {
        MessageBox(0, L"CreateDevice faield.", 0, 0);
        return false;
    }
    return true; 
}

inline bool D3DMainWindow::Display(float timeDelta)
{ 
    pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
        D3DCOLOR_COLORVALUE(0.0f, 1.0f, 0.0f, 1.0f), 1.0f, 0);

    pd3dDevice->BeginScene();

    // Render geometry here...

    pd3dDevice->EndScene();

    pd3dDevice->Present(NULL, NULL, NULL, NULL);
    return true; 
}

int D3DMainWindow::run()
{
    MSG msg;
    memset(&msg, 0, sizeof(msg));
    auto lastTime = (float)timeGetTime();
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            auto currTime = (float)timeGetTime();
            auto timeDelta = (currTime - lastTime)*0.001f;
            Display(timeDelta);
            lastTime = currTime;
        }
    }
    return msg.wParam;
}

LRESULT D3DMainWindow::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
		break;
	default:
		break;
	}
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

D3DMainWindow::~D3DMainWindow()
{
    Release(pD3D);
    Release(pd3dDevice);
    UnregisterClass(name, GetModuleHandle(NULL));
}
