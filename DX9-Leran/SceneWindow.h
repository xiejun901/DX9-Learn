#pragma once
#include "d3dMainWindow.h"
#include "StoneWallCube.h"
#include "Camera.h"
#include "SkyBox.h"
#include "Terrain.h"
#include "SnowBoy.h"
class SceneWindow :
	public D3DMainWindow
{
public:
	SceneWindow(const char * n, int w, int h):D3DMainWindow(n, w, h){}
	void Setup() override
	{
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI * 0.5f, // 90 - degree
			(float)width / (float)height,
			1.0f,
			3000.0f);
		pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj);

		cube = new StoneWallCube(pd3dDevice, "stone_wall_normal_map.bmp", "stone_wall.bmp");
		cube->init();
		skybox = new SkyBox(pd3dDevice);
		skybox->loadBackTexture("fadeaway_bk.tga");
		skybox->loadFrontTexture("fadeaway_ft.tga");
		skybox->loadDownTexture("fadeaway_dn.tga");
		skybox->loadUpTexture("fadeaway_up.tga");
		skybox->loadLeftTexture("fadeaway_lf.tga");
		skybox->loadRightTexture("fadeaway_rt.tga");
		pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		terrain = new Terrain(pd3dDevice, "c.raw", 128, 128, 10, 1.0f);
		//terrain->loadTexture("fadeaway_dn.tga");
		D3DXVECTOR3 lightDirection(0.0f, 1.0f, 1.0f);
		terrain->genTexture(&lightDirection);
		auto initCameraPos = D3DXVECTOR3(-144.0f, 25.0f, -156.0f);
		camera.setPosition(&initCameraPos);
        snowboy = new SnowBoy(pd3dDevice, "snowhead.jpg", "snowbody.jpg");
		D3DXMatrixTranslation(&matIniWorldSnowBoy, -194.0f, 35.0f, -149.0f);
		D3DXMatrixTranslation(&matIniWorldCube,    -194.0f, 29.0f, -149.0f);
		snowboy2 = new SnowBoy(pd3dDevice, "snowhead.jpg", "snowbody.jpg");
		D3DXMatrixTranslation(&matIniWorldSnowBoy2, -174.0f, 25.0f, -159.0f);

	}
	void Cleanup() override
	{

	}
	bool Display(float timeDelta) override
	{
		if (pd3dDevice)
		{
			D3DXVECTOR3 positionLast;
			camera.getPosition(&positionLast);
            cameraControl(timeDelta, 10.0f);
			D3DXVECTOR3 positionNow;
			camera.getPosition(&positionNow);
			D3DXVECTOR3 look;
			camera.getLook(&look);
			//positionNow = positionNow + look;
			//caculate the world transfer matrix of the object
			static float fAngle = 180;
			fAngle += 60 * timeDelta;
			// Wrap it around, if it gets too big
			while (fAngle > 360.0f) fAngle -= 360.0f;
			while (fAngle < 0.0f)   fAngle += 360.0f;
			D3DXMATRIX matRot;
			//Ðý×ª¾ØÕó
			D3DXMatrixRotationYawPitchRoll(&matRot,
				D3DXToRadian(fAngle),
				D3DXToRadian(0.0f),
				0.0f);
			D3DXMATRIX I;
			D3DXMatrixIdentity(&I);
			D3DXMATRIX matScale;
			D3DXMatrixScaling(&matScale, 5.0f, 5.0f, 5.0f);
			auto matWorldCube = matScale * matRot * matIniWorldCube;
			auto matWorldSnowBoy = matRot * matIniWorldSnowBoy;

			//if (cube->positionInnerObject(matWorldCube, positionNow))
			//	camera.setPosition(&positionLast);
			auto distance = D3DXVec3Length(&(positionNow - cubeIniPosition));
			if(distance < 10.2)
				camera.setPosition(&positionLast);

			D3DXMATRIX V;
			camera.getViewMatrix(&V);
			pd3dDevice->SetTransform(D3DTS_VIEW, &V);


			D3DXVECTOR3 lightPos(0.0f, 10000.0f, 10000.0f);
			pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE(0.35f, 0.53f, 0.7, 1.0f), 1.0f, 0);

			pd3dDevice->BeginScene();
			
			skybox->draw();
			terrain->draw(&I);
			cube->draw(matWorldCube, lightPos);
            
			D3DXVECTOR3 lightDirection(0.0f, -1.0f, -1.0f);
            snowboy->draw(matWorldSnowBoy, lightDirection);
			snowboy2->draw(matIniWorldSnowBoy2, lightDirection);

			pd3dDevice->EndScene();
			pd3dDevice->Present(0, 0, 0, 0);

		}
		return true;
	}
	~SceneWindow()
	{
		delete cube;
	}

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
	StoneWallCube *cube;
	Camera camera;
	SkyBox *skybox;
	Terrain *terrain;
    SnowBoy *snowboy;
	SnowBoy *snowboy2;
	D3DXVECTOR3 cubeIniPosition = D3DXVECTOR3(-194.0f, 29.0f, -149.0f);
	D3DXMATRIX matIniWorldSnowBoy;
	D3DXMATRIX matIniWorldCube;
	D3DXMATRIX matIniWorldSnowBoy2;


};

