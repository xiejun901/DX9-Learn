#ifndef _SNOW_BOY_H_
#define _SNOW_BOY_H_

#include<string>
#include <d3d9.h>
#include <d3dx9.h>
#include "d3dUtility.h"

class SnowBoy
{
public:
    SnowBoy(IDirect3DDevice9 *d, 
        const std::string &headTextureFileName, 
        const std::string &bodyTextureFileName): pDevice(d)
    {
        body = createTextureSphere(2.0f, 20, 20);
        head = createTextureSphere(1.0f, 20, 20);
        D3DXCreateTextureFromFile(
            pDevice,
            headTextureFileName.c_str(),
            &headTexture);
        D3DXCreateTextureFromFile(
            pDevice,
            bodyTextureFileName.c_str(),
            &bodyTexture);

    }
    void draw(const D3DXMATRIX &mWorld)
    {
 /*       pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);*/
        pDevice->SetRenderState(D3DRS_LIGHTING, false);
        pDevice->SetTexture(0, bodyTexture);
        pDevice->SetTransform(D3DTS_WORLD, &mWorld);
        body->DrawSubset(0);

        pDevice->SetTexture(0, headTexture);
        auto matWorld = headMatTrans*mWorld;
        pDevice->SetTransform(D3DTS_WORLD, &matWorld);
        head->DrawSubset(0);
        pDevice->SetRenderState(D3DRS_LIGHTING, true);
    }
    ~SnowBoy()
    {
        d3dUtil::Release(body);
        d3dUtil::Release(head);
    }

    ID3DXMesh *createTextureSphere(float radius, UINT slices, UINT stacks)
    {
        ID3DXMesh *mesh;
        auto hr = D3DXCreateSphere(
            pDevice,
            radius,
            slices,
            stacks,
            &mesh,
            0);
        if (FAILED(hr))
        {
            MessageBox(0, "create sphere failed", 0, 0);
            return nullptr;
        }
        ID3DXMesh *texMesh = nullptr;
        mesh->CloneMeshFVF(
            D3DXMESH_SYSTEMMEM,
            Vertex::FVF::FVF_Flags,
            pDevice,
            &texMesh);
        d3dUtil::Release(mesh);
         
        Vertex *pVertices = nullptr;
        hr = texMesh->LockVertexBuffer(0, (void **)&pVertices);
        auto numVertices = texMesh->GetNumVertices();
        for (int i = 0; i < numVertices; ++i)
        {
            float u = (asinf(pVertices->nx) / D3DX_PI + 0.5f)/2;
            float v = (asinf(pVertices->ny) / D3DX_PI + 0.5f) / 2;
            if (pVertices->nz <= 0)
            {
                pVertices->u = u;
                pVertices->v = v;
            }
            else
            {
                pVertices->u = u+0.5f;
                pVertices->v = v+0.5f;
            }
            
            pVertices->diffuse = D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
            ++pVertices;
        }
        texMesh->UnlockVertexBuffer();
        return texMesh;
    }
private:
    IDirect3DDevice9 *pDevice;
    ID3DXMesh *body;
    ID3DXMesh *head;
    IDirect3DTexture9 *headTexture;
    IDirect3DTexture9 *bodyTexture;
    static D3DXMATRIX headMatTrans;

    struct Vertex
    {
        float x, y, z;
        float nx, ny, nz;
        DWORD diffuse;
        float u, v;
        enum FVF
        {
            FVF_Flags = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1
        };
    };

};

#endif