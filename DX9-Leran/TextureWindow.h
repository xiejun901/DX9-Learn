#pragma once
#include "d3dMainWindow.h"
#include "cube.h"
class TextureWindow :
	public D3DMainWindow
{
public:
	TextureWindow(const char *n, int w, int h):D3DMainWindow(n,w,h){}
	void Setup() override
	{
		box = new Cube(pd3dDevice);

		D3DLIGHT9 light;
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Ambient = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
		light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
		light.Direction = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
		pd3dDevice->SetLight(0, &light);
		pd3dDevice->LightEnable(0, true);

		pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, true);

		auto hr = D3DXCreateTextureFromFile(
			pd3dDevice,
			"crate.jpg",
			&tex);
		
		pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI * 0.5f, // 90 - degree
			(float)width / (float)height,
			1.0f,
			1000.0f);
		pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj);


	}
	void Cleanup() override 
	{

	}
	bool Display(float timeDelta)
	{
		if (pd3dDevice)
		{
			// 
			// Update the scene: update camera position.
			//

			static float angle = (3.0f * D3DX_PI) / 2.0f;
			static float height = 2.0f;


			D3DXVECTOR3 position(cosf(angle) * 3.0f, height, sinf(angle) * 3.0f);
			D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
			D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
			D3DXMATRIX V;
			D3DXMatrixLookAtLH(&V, &position, &target, &up);

			pd3dDevice->SetTransform(D3DTS_VIEW, &V);

			angle += timeDelta;
			//
			// Draw the scene:
			//

			D3DXCOLOR white(D3DCOLOR_XRGB(255, 255, 255));
			D3DMATERIAL9 mtrl;
			mtrl.Ambient = white;
			mtrl.Diffuse = white;
			mtrl.Emissive = D3DXCOLOR(D3DCOLOR_XRGB(0, 0, 0));
			mtrl.Power = 2.0f;
			mtrl.Specular = white;


			pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			pd3dDevice->BeginScene();

			pd3dDevice->SetMaterial(&mtrl);
			pd3dDevice->SetTexture(0, tex);

			box->draw(0, 0, tex);

			pd3dDevice->EndScene();
			pd3dDevice->Present(0, 0, 0, 0);
		}
		return true;
	}
	~TextureWindow();
private:
	Cube *box;
	IDirect3DTexture9 *tex;
};

