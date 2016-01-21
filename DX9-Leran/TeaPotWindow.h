#ifndef _CUBE_WINDOW_H_
#define _CUBE_WINDOW_H_
#include "d3dMainWindow.h"
#include "d3dUtility.h"

class TeaPotWindow :
    public D3DMainWindow
{
public:
    TeaPotWindow(const char * _name, int _width, int _height):D3DMainWindow(_name, _width, _height){}
    void Setup() override 
    {
        D3DXCreateTeapot(pd3dDevice, &Teapot, 0);

        D3DXVECTOR3 position(0.0f, 0.0f, -5.0f);
        D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
        D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
        D3DXMATRIX v;
        D3DXMatrixLookAtLH(&v, &position, &target, &up);
        pd3dDevice->SetTransform(D3DTS_VIEW, &v);
        D3DXMATRIX proj;
        D3DXMatrixPerspectiveFovLH(
            &proj,
            D3DX_PI * 0.5f,
            (float)width / (float)height,
            1.0f,
            1000.0f
            );
        pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj);

        pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
        pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
        pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    }
    bool Display(float timeDelta) override
    {
        if (pd3dDevice)
        {
            pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE(0.0f, 1.0f, 0.0f, 1.0f), 1.0f, 0);
            pd3dDevice->BeginScene();
            D3DXMATRIX  ry;
            static float y = 0.0f;
            D3DXMatrixRotationY(&ry, y);
            y += timeDelta;
            if (y >= 6.28f)
            {
                y = 0.0f;
            }
            pd3dDevice->SetTransform(D3DTS_WORLD, &ry);

            Teapot->DrawSubset(0);

            pd3dDevice->EndScene();
            pd3dDevice->Present(0, 0, 0, 0);
        }
        return true;
    }
    ~TeaPotWindow()
    {
        Release(vb);
        Release(ib);
        Release(Teapot);
    }
private:
    IDirect3DVertexBuffer9 *vb = nullptr;
    IDirect3DIndexBuffer9 *ib = nullptr;
    ID3DXMesh *Teapot = nullptr;
};
#endif

