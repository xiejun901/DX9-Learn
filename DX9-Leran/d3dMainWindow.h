#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include<windows.h>
#include <d3d9.h>
#include <d3dx9.h>
class D3DMainWindow
{
public:
    D3DMainWindow(const char *_name, int w, int h):name(_name), width(w), height(h) {}
    D3DMainWindow(): name("non-name window"), width(640), height(480){}
    void Init();
    bool InitWindow();
    bool InitD3D();
    int run();
    virtual ~D3DMainWindow();

    static LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    virtual void Setup() {}
    virtual void Cleanup() {}
    virtual bool Display(float timeDelta);
protected:
    const char *name;
    HWND hWnd = nullptr;
    LPDIRECT3D9 pD3D = nullptr;
    LPDIRECT3DDEVICE9 pd3dDevice = nullptr;
    int width, height;

};
#endif