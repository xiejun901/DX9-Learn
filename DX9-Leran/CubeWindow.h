#ifndef _CUBE_WINDOW_H_
#define _CUBE_WINDOW_H_
#include "d3dMainWindow.h"
struct Vertex {
    Vertex() {}
    Vertex(float x, float y, float z) :_x(x), _y(y), _z(z) {}
    float _x, _y, _z;
    static const DWORD FVF;
};
class CubeWindow :
    public D3DMainWindow
{
public:
    CubeWindow(const wchar_t * _name, int _width, int _height):D3DMainWindow(_name, _width, _height){}
    void Setup() override 
    {
        //pd3dDevice->CreateVertexBuffer(
        //    8 * sizeof(Vertex),
        //    D3DUSAGE_WRITEONLY,
        //    Vertex::FVF,
        //    D3DPOOL_MANAGED,
        //    &vb,
        //    0);
        //pd3dDevice->CreateIndexBuffer(
        //    36 * sizeof(WORD),
        //    D3DUSAGE_WRITEONLY,
        //    D3DFMT_INDEX16,
        //    D3DPOOL_MANAGED,
        //    &ib,
        //    0);
        //Vertex *vertices;
        //vb->Lock(0, 0, (void **)&vertices, 0);
        //vertices[0] = Vertex(-1.0f, -1.0f, -1.0f);
        //vertices[1] = Vertex(-1.0f, 1.0f, -1.0f);
        //vertices[2] = Vertex(1.0f, 1.0f, -1.0f);
        //vertices[3] = Vertex(1.0f, -1.0f, -1.0f);
        //vertices[4] = Vertex(-1.0f, -1.0f, 1.0f);
        //vertices[5] = Vertex(-1.0f, 1.0f, 1.0f);
        //vertices[6] = Vertex(1.0f, 1.0f, 1.0f);
        //vertices[7] = Vertex(1.0f, -1.0f, 1.0f);
        //vb->Unlock();

        //// define the triangles of the cube:
        //WORD* indices = 0;
        //ib->Lock(0, 0, (void**)&indices, 0);

        //// front side
        //indices[0] = 0; indices[1] = 1; indices[2] = 2;
        //indices[3] = 0; indices[4] = 2; indices[5] = 3;

        //// back side
        //indices[6] = 4; indices[7] = 6; indices[8] = 5;
        //indices[9] = 4; indices[10] = 7; indices[11] = 6;

        //// left side
        //indices[12] = 4; indices[13] = 5; indices[14] = 1;
        //indices[15] = 4; indices[16] = 1; indices[17] = 0;

        //// right side
        //indices[18] = 3; indices[19] = 2; indices[20] = 6;
        //indices[21] = 3; indices[22] = 6; indices[23] = 7;

        //// top
        //indices[24] = 1; indices[25] = 5; indices[26] = 6;
        //indices[27] = 1; indices[28] = 6; indices[29] = 2;

        //// bottom
        //indices[30] = 4; indices[31] = 0; indices[32] = 3;
        //indices[33] = 4; indices[34] = 3; indices[35] = 7;

        //ib->Unlock();
        D3DXCreateTeapot(pd3dDevice, &Teapot, 0);

        D3DXVECTOR3 position(0.0f, 0.0f, -3.0f);
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
        pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
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
private:
    IDirect3DVertexBuffer9 *vb = nullptr;
    IDirect3DIndexBuffer9 *ib = nullptr;
    ID3DXMesh *Teapot = nullptr;
};
#endif

