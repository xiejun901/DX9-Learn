#pragma once
#include "d3dMainWindow.h"
#include "SnowBoy.h"
#include "Camera.h"
class SnowBoyWindow :
    public D3DMainWindow
{
public:
    SnowBoyWindow();
    SnowBoyWindow(const char *n, int w, int h):D3DMainWindow(n,w,h){}
    void Setup() override
    {
        D3DXMATRIX proj;
        D3DXMatrixPerspectiveFovLH(
            &proj,
            D3DX_PI * 0.5f,
            (float)width / (float)height,
            1.0f,
            1000.0f
            );
        pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj);

        snowBoy = new SnowBoy(pd3dDevice, "snow.jpg", "snow.jpg");

        //pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
        pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
        pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    }
    void Cleanup() override
    {

    }
    bool Display(float timeDelta) override
    {
        cameraControl(timeDelta, 10.0f);
        pd3dDevice->BeginScene();
        D3DXMATRIX world;
        D3DXMatrixTranslation(&world, 0.0f, -1.0f, 10.0f);
        D3DXMATRIX V;
        camera.getViewMatrix(&V);
        pd3dDevice->SetTransform(D3DTS_VIEW, &V);
        pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);
        snowBoy->draw(world, D3DXVECTOR3(1.0f, 1.0f, 1.0f));
        pd3dDevice->EndScene();
        pd3dDevice->Present(0, 0, 0, 0);
        return true;
    }
    ~SnowBoyWindow();
private:
    void cameraControl(float timeDelta, float moveVelocity = 100.0f)
    {
        unsigned char keys[256];
        GetKeyboardState(keys);

        // key W is Down
        if (keys['W'] & 0x80)
            camera.walk(moveVelocity*timeDelta);

        //key S is Down
        if (keys['S'] & 0x80)
            camera.walk(-moveVelocity*timeDelta);

        //key A is Down
        if (keys['A'] & 0x80)
            camera.strafe(-moveVelocity * timeDelta);
        //key D is Down
        if (keys['D'] & 0x80)
            camera.strafe(moveVelocity * timeDelta);

        if (keys[VK_LEFT] & 0x80)
            camera.yaw(-timeDelta);

        if (keys[VK_RIGHT] & 0x80)
            camera.yaw(timeDelta);

        if (keys[VK_UP] & 0x80)
            camera.pitch(-timeDelta);

        if (keys[VK_DOWN] & 0x80)
            camera.pitch(timeDelta);

        if (keys['Q'] & 0x80)
            camera.fly(moveVelocity * timeDelta);

        if (keys['E'] & 0x80)
            camera.fly(-moveVelocity * timeDelta);
    }
private:
    SnowBoy *snowBoy;
    Camera camera;

};

