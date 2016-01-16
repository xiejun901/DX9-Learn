#include<windows.h>
#include"mainWindow.h"

INT WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, INT)
{
    MainWindow app;
    app.InitWindow();
    app.run();
}
