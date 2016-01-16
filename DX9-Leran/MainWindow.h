#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H

#include<windows.h>
#include <d3d9.h>
#include <d3dx9.h>
class D3DMainWindow
{
public:
    D3DMainWindow(const wchar_t *_name, int w, int h):name(_name), width(w), height(h) {}
    void Init();
    bool InitWindow();
    bool InitD3D();
    int run();
    virtual ~D3DMainWindow();

    static LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void Setup() {}
    void Cleanup() {}
    bool Display(float timeDelta);
protected:
    const wchar_t *name;
    HWND hWnd = nullptr;
    LPDIRECT3D9 pD3D = nullptr;
    LPDIRECT3DDEVICE9 pd3dDevice = nullptr;
    int width, height;

};
#endif