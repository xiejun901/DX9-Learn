#pragma once
#include "d3dMainWindow.h"
class LightWindow :
    public D3DMainWindow
{
public:
    LightWindow(const wchar_t *n, int w, int h): D3DMainWindow(n,w,h){}
    LightWindow();
    void Setup() override
    {
        D3DXCreateTeapot(pd3dDevice, &objects[0], 0);
        D3DXCreateSphere(pd3dDevice, 1.0f, 20, 20, &objects[1], 0);
        D3DXCreateTorus(pd3dDevice, 0.5f, 1.0f, 20, 20, &objects[2], 0);
        D3DXCreateCylinder(pd3dDevice, 0.5f, 0.5f, 2.0f, 20, 20, &objects[3], 0);

    }
    void Cleanup() override
    {

    }
    bool Display(float timeDelata) override
    {

    }
    ~LightWindow();

    ID3DXMesh * objects[4] = { 0, 0, 0, 0 };

};

