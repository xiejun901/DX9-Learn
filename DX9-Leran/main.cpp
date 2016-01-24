#include<windows.h>
#include<d3dx9.h>
#include "d3dmainWindow.h"
#include "TeapotWindow.h"
#include "ColorTriangle.h"
#include "LightWindow.h"
#include "TextureWindow.h"
#include "CameraWindow.h"
#include "TerrainWindow.h"
#include "SkyBoxWindow.h"
#include "Dot3CubeWindow.h"
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
