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
		skybox = new SkyBox(pd3dDevice, 
			"fadeaway_up.tga", 
			"fadeaway_dn.tga", 
			"fadeaway_lf.tga", 
			"fadeaway_rt.tga", 
			"fadeaway_ft.tga", 
			"fadeaway_bk.tga");
		skybox->init();
		pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		terrain = new Terrain(pd3dDevice, "c.raw", 128, 128, 10, 1.0f);
		//terrain->loadTexture("fadeaway_dn.tga");
		D3DXVECTOR3 lightDirection(0.0f, 1.0f, 1.0f);
		terrain->genTexture(&lightDirection);
		auto initCameraPos = D3DXVECTOR3(-144.0f, 25.0f, -156.0f);
		camera.setPosition(&initCameraPos);
        snowboy = new SnowBoy(pd3dDevice);
		snowboy->init();
		D3DXMatrixTranslation(&matIniWorldSnowBoy, -194.0f, 35.0f, -149.0f);
		D3DXMatrixTranslation(&matIniWorldCube,    -194.0f, 29.0f, -149.0f);
		snowboy2 = new SnowBoy(pd3dDevice);
		snowboy2->init();
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
			static float fAngle = 0;
			//fAngle += 60 * timeDelta;
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
			//auto distance = D3DXVec3Length(&(positionNow - cubeIniPosition));
			//if(distance < 10.2)
			//	camera.setPosition(&positionLast);
			if (checkCameraCollision(positionNow))
			{
				camera.setPosition(&positionLast);
			}
			D3DXMATRIX V;
			camera.getViewMatrix(&V);
			pd3dDevice->SetTransform(D3DTS_VIEW, &V);

			D3DXVECTOR3 lightDirection(0.0f, -1.0f, -1.0f);
			pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE(0.35f, 0.53f, 0.7, 1.0f), 1.0f, 0);

			pd3dDevice->BeginScene();
			
			skybox->draw(&I);
			terrain->draw(&I);
			cube->draw(&matWorldCube, &lightDirection);
            

            snowboy->draw(&matWorldSnowBoy, &lightDirection);
			snowboy2->draw(&matIniWorldSnowBoy2, &lightDirection);

			pd3dDevice->EndScene();
			pd3dDevice->Present(0, 0, 0, 0);

		}
		return true;
	}
	~SceneWindow()
	{
		delete cube;
		delete snowboy;
		delete snowboy2;
		delete skybox;
		delete terrain;
	}

private:
    void cameraControl(float timeDelta, float moveVelocity = 100.0f)
    {
		POINT mousePosit;
		GetCursorPos(&mousePosit);
		ScreenToClient(hWnd, &mousePosit);
		static POINT ptCurrentMousePosit;
		static POINT ptLastMousePosit;
		ptCurrentMousePosit.x = mousePosit.x;
		ptCurrentMousePosit.y = mousePosit.y;

		D3DXMATRIX matRotation;

		if (bMousing)
		{
			int nXDiff = (ptCurrentMousePosit.x - ptLastMousePosit.x);
			int nYDiff = (ptCurrentMousePosit.y - ptLastMousePosit.y);

			if (nYDiff != 0)
			{
				camera.pitch(D3DXToRadian((float)nYDiff / 3.0f));
			}

			if (nXDiff != 0)
			{
				camera.yaw(D3DXToRadian((float)nXDiff / 3.0f));
			}
		}
		ptLastMousePosit = ptCurrentMousePosit;
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

		D3DXVECTOR3 position;
		//camera.getPosition(&position);
		////position.y -= timeDelta;
  //      terrain->transfrom(&position, &position);
		//camera.setPosition(&position);
		if (!(keys[VK_SPACE] & 0x80))
		{
			//D3DXVECTOR3 position;
			//camera.getPosition(&position);
			//terrain->transfrom(&position, &position);
			//camera.setPosition(&position);
            //camera.getPosition(&position);
            //position.y -= timeDelta;
            //terrain->transfrom(&position, &position);
            //camera.setPosition(&position);
            //camera.getPosition(&position);
            ////position.y -= timeDelta;
            // terrain->transfrom(&position, &position);
            //camera.setPosition(&position);
            D3DXVECTOR3 position;
            camera.getPosition(&position);
            position.y -= 50*timeDelta;
            camera.setPosition(&position);
            auto height = terrain->getHeight(position.x,position.z);
            if (position.y < height + 5.0f)
                position.y = height + 5.0f;
            camera.setPosition(&position);
		}
        else
        {
            D3DXVECTOR3 position;
            camera.getPosition(&position);
            position.y += 20 * timeDelta;
            camera.setPosition(&position);
        }
    }
private:
	StoneWallCube *cube;
	Camera camera;
	SkyBox *skybox;
	Terrain *terrain;
    SnowBoy *snowboy;
	SnowBoy *snowboy2;
	D3DXVECTOR3 cubeIniPosition  = D3DXVECTOR3(-194.0f, 29.0f, -149.0f);
	D3DXVECTOR3 snowBoy1Position = D3DXVECTOR3(-194.0f, 35.0f, -149.0f);
	D3DXVECTOR3 snowBoy2Position = D3DXVECTOR3(-174.0f, 25.0f, -159.0f);
	D3DXMATRIX matIniWorldSnowBoy;
	D3DXMATRIX matIniWorldCube;
	D3DXMATRIX matIniWorldSnowBoy2;

	bool checkCameraCollision(const D3DXVECTOR3 &cameraPositionNow)
	{
		//auto terrainHeight = terrain->getHeight(cameraPositionNow.x, cameraPositionNow.z);
		//if (cameraPositionNow.y - terrainHeight < 2.0f)
		//	return true;
		//auto distance = D3DXVec3Length(&(cameraPositionNow - cubeIniPosition));
		//if (distance < 10.2)
		//	return true;
		//return false;
        auto cubeX = D3DXVECTOR3(5.0f, 0.0f, 0.0f) + cubeIniPosition;
        auto cubeY = D3DXVECTOR3(0.0f, 5.0f, 0.0f) + cubeIniPosition;
        auto cubeZ = D3DXVECTOR3(0.0f, 0.0f, 5.0f) + cubeIniPosition;
        D3DXVECTOR3 position;
        camera.getPosition(&position);
        return d3dUtil::checkIntersectionBoxSphere(cubeIniPosition, cubeX, cubeY, cubeZ, position, 1.0f);
        
	}


};

