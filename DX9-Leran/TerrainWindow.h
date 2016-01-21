#pragma once
#include "d3dMainWindow.h"
#include "Terrain.h"
#include "Camera.h"
class TerrainWindow :
	public D3DMainWindow
{
public:
	TerrainWindow();
	TerrainWindow(const wchar_t *n, int w, int h) :D3DMainWindow(n, w, h) {

	}
	void Setup() override
	{
		D3DXVECTOR3 lightDirection(0.0f, 1.0f, 0.0f);
		terrain = new Terrain(pd3dDevice, "c.raw", 128, 128, 10, 1.0f);

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

			unsigned char keys[256];
			GetKeyboardState(keys);

			// key W is Down
			if (keys['W'] & 0x80)
				camera.walk(100.0f*timeDelta);

			//key S is Down
			if (keys['S'] & 0x80)
				camera.walk(-100.0f*timeDelta);

			//key A is Down
			if (keys['A'] & 0x80)
				camera.strafe(-100.0f * timeDelta);
			//key D is Down
			if (keys['D'] & 0x80)
				camera.strafe(100.0f * timeDelta);

			if (keys[VK_LEFT] & 0x80)
				camera.yaw(-timeDelta);

			if (keys[VK_RIGHT] & 0x80)
				camera.yaw(timeDelta);

			if (keys[VK_UP] & 0x80)
				camera.pitch(-timeDelta);

			if (keys[VK_DOWN] & 0x80)
				camera.pitch(timeDelta);

			if (keys['Q'] & 0x80)
				camera.fly(100.0f * timeDelta);

			if (keys['E'] & 0x80)
				camera.fly(-100.0f * timeDelta);

			D3DXMATRIX V;
			camera.getViewMatrix(&V);
			pd3dDevice->SetTransform(D3DTS_VIEW, &V);
			pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0);
			pd3dDevice->BeginScene();
			D3DXMATRIX I;
			D3DXMatrixIdentity(&I);
			terrain->draw(&I);
			pd3dDevice->EndScene();
			pd3dDevice->Present(0, 0, 0, 0);
		}
		return true;
	}
	~TerrainWindow();

	Terrain *terrain;
	Camera camera;
};

