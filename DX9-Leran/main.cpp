#include<windows.h>
#include<d3dx9.h>
#include "SceneWindow.h"
int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
    SceneWindow app("WINDOW", 640, 480);
    app.Init();
    app.Setup();
    app.run();   
}
