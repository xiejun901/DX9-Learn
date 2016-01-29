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

		cube = new StoneWallCube(pd3dDevice, "stone_wall_normal_map.bmp", "stone_wall.bmp", 10.0f);
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
		auto initCameraPos = D3DXVECTOR3(cubeIniPosition.x, cubeIniPosition.y, cubeIniPosition.z+10.0f);
		camera.setPosition(&initCameraPos);
        snowboy = new SnowBoy(pd3dDevice);
		snowboy->init();
		D3DXMatrixTranslation(&matIniWorldSnowBoy, snowBoy1Position.x, snowBoy1Position.y, snowBoy1Position.z);
		D3DXMatrixTranslation(&matIniWorldCube, cubeIniPosition.x, cubeIniPosition.y, cubeIniPosition.z);
		snowboy2 = new SnowBoy(pd3dDevice);
		snowboy2->init();
		D3DXMatrixTranslation(&matIniWorldSnowBoy2, snowBoy2Position.x, snowBoy2Position.y, snowBoy2Position.z);

	}
	void Cleanup() override
	{

	}
	bool Display(float timeDelta) override
	{
		if (pd3dDevice)
		{
            cameraControl(timeDelta, 10.0f);
			static float fAngle = 45.0f;
			fAngle += 60 * timeDelta;
			// Wrap it around, if it gets too big
			while (fAngle > 360.0f) fAngle -= 360.0f;
			while (fAngle < 0.0f)   fAngle += 360.0f;
			D3DXMATRIX matRot;
			//旋转矩阵
			D3DXMatrixRotationYawPitchRoll(&matRot,
				D3DXToRadian(fAngle),
				D3DXToRadian(0.0f),
				0.0f);
			D3DXMATRIX I;
			D3DXMatrixIdentity(&I);
			auto matWorldCube = matRot * matIniWorldCube;
			auto matWorldSnowBoy = matRot * matIniWorldSnowBoy;

			D3DXMATRIX V;
			camera.getViewMatrix(&V);
			pd3dDevice->SetTransform(D3DTS_VIEW, &V);

			D3DXVECTOR3 lightDirection(0.0f, -1.0f, -1.0f);
			pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_COLORVALUE(0.35f, 0.53f, 0.7, 1.0f), 1.0f, 0);

			pd3dDevice->BeginScene();
			
			skybox->draw(&I);
			terrain->draw(&I);
			cube->draw(&matWorldCube, &lightDirection);

            D3DXVec3TransformNormal(&cubeX, &cubeXIni, &matRot);
            D3DXVec3TransformNormal(&cubeY, &cubeYIni, &matRot);
            D3DXVec3TransformNormal(&cubeZ, &cubeZIni, &matRot);

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
		D3DXVECTOR3 positionLast;
		camera.getPosition(&positionLast);
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

		if (!(keys[VK_SPACE] & 0x80))
		{
			D3DXVECTOR3 position;
			camera.getPosition(&position);
			if (terrain->intersection(position, 5.0f))
			{
				camera.fly(0.3f);
				camera.getPosition(&position);
				if (terrain->intersection(position, 5.0f))
				{
					camera.setPosition(&positionLast);
				}
			}
			if(checkCameraCollision(position))
				camera.setPosition(&positionLast);
			if (canFallDown())
			{
				camera.fly(-0.3f);
			}
		}
        else
        {
            D3DXVECTOR3 position;
            camera.getPosition(&position);
            position.y += 40.0f * timeDelta;
            camera.setPosition(&position);
        }
    }
	bool canFallDown()
	{
		D3DXVECTOR3 position;
		camera.getPosition(&position);
		return !(d3dUtil::checkIntersectionBoxSphere(cubeIniPosition, cubeX, cubeY, cubeZ, position, 2.5f) || terrain->intersection(position, 6.0f));
	}
private:
	StoneWallCube *cube;
	Camera camera;
	SkyBox *skybox;
	Terrain *terrain;
    SnowBoy *snowboy;
	SnowBoy *snowboy2;
	D3DXVECTOR3 cubeIniPosition  = D3DXVECTOR3(-194.0f, 35.0f, -149.0f);
	D3DXVECTOR3 snowBoy1Position = D3DXVECTOR3(-194.0f, 35.0f, -149.0f);
	D3DXVECTOR3 snowBoy2Position = D3DXVECTOR3(-174.0f, 25.0f, -159.0f);
	D3DXMATRIX matIniWorldSnowBoy;
	D3DXMATRIX matIniWorldCube;
	D3DXMATRIX matIniWorldSnowBoy2;
	D3DXVECTOR3 cubeX = D3DXVECTOR3(10.0f, 0.0f, 0.0f);
	D3DXVECTOR3 cubeY = D3DXVECTOR3(0.0f, 10.0f, 0.0f);
	D3DXVECTOR3 cubeZ = D3DXVECTOR3(0.0f, 0.0f, 10.0f);
	//立方体初始XYZ轴向量
	D3DXVECTOR3 cubeXIni = D3DXVECTOR3(10.0f, 0.0f, 0.0f);
	D3DXVECTOR3 cubeYIni = D3DXVECTOR3(0.0f, 10.0f, 0.0f);
	D3DXVECTOR3 cubeZIni = D3DXVECTOR3(0.0f, 0.0f, 10.0f);

	bool checkCameraCollision(const D3DXVECTOR3 &cameraPositionNow)
	{
        D3DXVECTOR3 position;
        camera.getPosition(&position);
        return d3dUtil::checkIntersectionBoxSphere(cubeIniPosition, cubeX, cubeY, cubeZ, position, 1.7f);    
	}

    bool cameraCollisionToCube()
    {
        D3DXVECTOR3 position;
        camera.getPosition(&position);
        return d3dUtil::checkIntersectionBoxSphere(cubeIniPosition, cubeX, cubeY, cubeZ, position, 1.7f);
    }

};

