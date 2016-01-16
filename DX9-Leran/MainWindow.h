#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_


#include<windows.h>

class MainWindow
{
public:
    MainWindow() = default;
    bool InitWindow()
    {
        WNDCLASSEX wc =
        {
            sizeof(WNDCLASSEX), 
            CS_CLASSDC, 
            MainWindow::MsgProc,  //��Ϣ�ص�
            0L, 
            0L,
            GetModuleHandle(NULL), 
            NULL, 
            NULL, 
            NULL, 
            NULL,
            name, 
            NULL
        };
        if (!RegisterClassEx(&wc))
        {
            MessageBox(0, L"RegisterClass() - FAILED", 0, 0);
            return false;
        }

        HWND hWnd = CreateWindow(
            name,  // ����
            name, //��������
            WS_OVERLAPPEDWINDOW,  //���ڷ��
            CW_USEDEFAULT, // ˮƽλ��
            CW_USEDEFAULT,  // ��ֱλ��
            CW_USEDEFAULT,// ���
            CW_USEDEFAULT, //�߶�
            NULL,
            NULL, 
            wc.hInstance, 
            NULL);
        if (!hWnd)
        {
            MessageBox(0, L"CreateWindow() - FAILED", 0, 0);
            return false;
        }
        ShowWindow(hWnd, SW_SHOWDEFAULT);
        UpdateWindow(hWnd);
        return 0;
    }
    int run()
    {
        MSG msg;
        while (true)
        {
            if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                    break;
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {

            }
        }
        return msg.wParam;
    }

    static LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }


    virtual ~MainWindow()
    {
        UnregisterClass(name, GetModuleHandle(NULL));
    }

private:
    HWND hWnd = 0;
    wchar_t *name = L"window";
};
#endif