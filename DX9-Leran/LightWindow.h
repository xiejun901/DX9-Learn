#ifndef _LIGHT_WINDOW_H_
#define _LIGHT_WINDOW_H_
#pragma once
#include "d3dMainWindow.h"
#include "d3dUtility.h"
class LightWindow :
    public D3DMainWindow
{
public:
    LightWindow(const char *n, int w, int h): D3DMainWindow(n,w,h){}
    LightWindow();
    void Setup() override
    {
		// 创建四个形状
        D3DXCreateTeapot(pd3dDevice, &objects[0], 0);
        D3DXCreateSphere(pd3dDevice, 1.0f, 20, 20, &objects[1], 0);
        D3DXCreateTorus(pd3dDevice, 0.5f, 1.0f, 20, 20, &objects[2], 0);
        D3DXCreateCylinder(pd3dDevice, 0.5f, 0.5f, 2.0f, 20, 20, &objects[3], 0);

		//创建四个位置用于世界变换
		D3DXMatrixTranslation(&word[0], 0.0f, 2.0f, 0.0f);
		D3DXMatrixTranslation(&word[1], 0.0f, -2.0f, 0.0f);
		D3DXMatrixTranslation(&word[2], 2.0f, 0.0f, 0.0f);
		D3DXMatrixTranslation(&word[3], -2.0f, 0.0f, 0.0f);

		//创建四种材料
		D3DXCOLOR white(D3DCOLOR_XRGB(255, 255, 255));
		D3DXCOLOR red(D3DCOLOR_XRGB(255, 0, 0));
		D3DXCOLOR green(D3DCOLOR_XRGB(0, 255, 0));
		D3DXCOLOR blue(D3DCOLOR_XRGB(0, 0, 255));
		D3DXCOLOR black(D3DCOLOR_XRGB(0, 0, 0));
		D3DXCOLOR yellow(D3DCOLOR_XRGB(255, 255, 0));
		D3DXCOLOR cyan(D3DCOLOR_XRGB(0, 255, 255));
		D3DXCOLOR magenta(D3DCOLOR_XRGB(255, 0, 255));
		mtrls[0].Ambient = white;
		mtrls[0].Diffuse = white;
		mtrls[0].Specular = white;
		mtrls[0].Emissive = black;
		mtrls[0].Power = 2.0f;

		mtrls[1].Ambient = red;
		mtrls[1].Diffuse = red;
		mtrls[1].Specular = red;
		mtrls[1].Emissive = black;
		mtrls[1].Power = 2.0f;

		mtrls[2].Ambient = green;
		mtrls[2].Diffuse = green;
		mtrls[2].Specular = green;
		mtrls[2].Emissive = black;
		mtrls[2].Power = 2.0f;
		
		mtrls[3].Ambient = blue;
		mtrls[3].Diffuse = blue;
		mtrls[3].Specular = blue;
		mtrls[3].Emissive = blue;
		mtrls[3].Power = 2.0f;

		D3DXVECTOR3 dir(1.0, 0.0f, 0.25f);
		D3DXCOLOR c = white;
		D3DLIGHT9 light;
		ZeroMemory(&light, sizeof(light));
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Ambient = c * 0.6f;
		light.Diffuse = c;
		light.Specular = c * 0.6f;
		light.Direction = dir;

		pd3dDevice->SetLight(0, &light);
		pd3dDevice->LightEnable(0, true);

		pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, false);
		
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI * 0.25f, // 45 - degree
			(float)width / (float)height,
			1.0f,
			1000.0f);
		pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj);


    }
    void Cleanup() override
    {

    }
    bool Display(float timeDelta) override
    {
		if (pd3dDevice)
		{
			// 
			// Update the scene: update camera position.
			//

			static float angle = (3.0f * D3DX_PI) / 2.0f;
			static float height = 5.0f;

			D3DXVECTOR3 position(cosf(angle) * 7.0f, height, sinf(angle) * 7.0f);
			D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
			D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
			D3DXMATRIX V;
			D3DXMatrixLookAtLH(&V, &position, &target, &up);

			pd3dDevice->SetTransform(D3DTS_VIEW, &V);
			angle += timeDelta;
			//
			// Draw the scene:
			//
			pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
			pd3dDevice->BeginScene();

			for (int i = 0; i < 4; i++)
			{
				// set material and world matrix for ith object, then render
				// the ith object.
				pd3dDevice->SetMaterial(&mtrls[i]);
				pd3dDevice->SetTransform(D3DTS_WORLD, &word[i]);
				objects[i]->DrawSubset(0);
			}

			pd3dDevice->EndScene();
			pd3dDevice->Present(0, 0, 0, 0);
		}
		return true;
    }
	~LightWindow() 
	{
		for (int i = 0; i < 4; i++)
		{
			Release(objects[i]);
		}
	}

    ID3DXMesh * objects[4] = { 0, 0, 0, 0 };
	D3DXMATRIX word[4] ;
	D3DMATERIAL9 mtrls[4] ;

};

#endif