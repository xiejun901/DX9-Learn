#ifndef _CAMERA_WINDOW_H_
#define _CAMERA_WINDOW_H_
//用来测试Camera效果的类

#include "d3dMainWindow.h"
#include "d3dUtility.h"
#include "Camera.h"
class CameraWindow :
	public D3DMainWindow
{
public:
	CameraWindow(const char *n, int w, int h):D3DMainWindow(n,w,h){}
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
				camera.walk(timeDelta);

			//key S is Down
			if (keys['S'] & 0x80)
				camera.walk(-timeDelta);

			//key A is Down
			if (keys['A'] & 0x80)
				camera.strafe(-timeDelta);
			//key D is Down
			if (keys['D'] & 0x80)
				camera.strafe(timeDelta);

			pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);
			pd3dDevice->BeginScene();

			D3DXMATRIX v;
			camera.getViewMatrix(&v);
			pd3dDevice->SetTransform(D3DTS_VIEW, &v);

			D3DXMATRIX WorldMatrix;
			D3DXMatrixTranslation(&WorldMatrix, 0.0f, 0.0f, 3.0f);
			pd3dDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);
			Teapot->DrawSubset(0);

			pd3dDevice->EndScene();
			pd3dDevice->Present(0, 0, 0, 0);
		}
		return true;
	}
	~CameraWindow()
	{
		d3dUtil::Release(Teapot);
	}

	ID3DXMesh *Teapot = nullptr;
	Camera camera;
};

#endif // !_CAMERA_WINDOW_H_