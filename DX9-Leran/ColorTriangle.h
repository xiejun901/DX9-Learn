#ifndef _COLOR_TRIANGLE_H_
#define _COLOR_TRIANGLE_H_
#include "d3dMainWindow.h"
#include "d3dUtility.h"
struct ColorVertex {
    float _x, _y, _z;
    D3DCOLOR color;
    ColorVertex(float x, float y, float z, D3DCOLOR c) :_x(x), _y(y), _z(z), color(c)
    {

    }
    static const DWORD FVF;
};
class ColorTrangle :
    public D3DMainWindow
{
public:
    void Setup() override
    {
        pd3dDevice->CreateVertexBuffer(
            3 * sizeof(ColorVertex),
            D3DUSAGE_WRITEONLY,
            ColorVertex::FVF,
            D3DPOOL_MANAGED,
            &triangle,
            0);
        ColorVertex *v;
        triangle->Lock(0, 0, (void **)&v, 0);


        v[0] = ColorVertex(-1.0f, 0.0f, 2.0f, D3DCOLOR_XRGB(255, 0, 0));
        v[1] = ColorVertex(0.0f, 1.0f, 2.0f, D3DCOLOR_XRGB(0, 255, 0));
        v[2] = ColorVertex(1.0f, 0.0f, 2.0f, D3DCOLOR_XRGB(0, 0, 255));

        triangle->Unlock();
        D3DXMATRIX proj;
        D3DXMatrixPerspectiveFovLH(
            &proj,
            D3DX_PI * 0.5f,
            (float)width / (float)height,
            1.0f,
            1000.0f);
        pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj);

        pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);

    }
    void Cleanup() override
    {

    }
    bool Display(float timeDelta) override
    {
        if (pd3dDevice)
        {
            D3DXMATRIX WorldMatrix;

            pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
            pd3dDevice->BeginScene();

            pd3dDevice->SetFVF(ColorVertex::FVF);
            pd3dDevice->SetStreamSource(0, triangle, 0, sizeof(ColorVertex));

            // draw the triangle to the left with flat shading
            D3DXMatrixTranslation(&WorldMatrix, -1.25f, 0.0f, 0.0f);
            pd3dDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);

            pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
            pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

            // draw the triangle to the right with gouraud shading
            D3DXMatrixTranslation(&WorldMatrix, 1.25f, 0.0f, 0.0f);
            pd3dDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);

            pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
            pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

            pd3dDevice->EndScene();
            pd3dDevice->Present(0, 0, 0, 0);
        }
        return true;
    }
    ColorTrangle(const char *n, int w, int h):D3DMainWindow(n, w, h) {}
    ~ColorTrangle() 
    {
        Release(triangle);
    }
    IDirect3DVertexBuffer9 *triangle = 0;

};

#endif