#include<windows.h>
#include<d3dx9.h>
#include"mainWindow.h"

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
    D3DMainWindow app(L"WINDOW", 640, 480);
    app.InitWindow();
    app.run();
    
}
