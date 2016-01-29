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
    void Setup() override;
    void Cleanup() override;
    bool Display(float timeDelta) override;
    ~SceneWindow();

private:
    void cameraControl(float timeDelta, float moveVelocity = 100.0f);
    bool canFallDown();
    bool checkCameraCollision(const D3DXVECTOR3 &cameraPositionNow);
private:
	StoneWallCube *cube;
	Camera camera;
	SkyBox *skybox;
	Terrain *terrain;
    SnowBoy *snowboy;
	SnowBoy *snowboy2;
    ID3DXFont* Font = 0;

	D3DXVECTOR3 cubeIniPosition  = D3DXVECTOR3(-194.0f, 35.0f, -149.0f);
	D3DXVECTOR3 snowBoy1Position = D3DXVECTOR3(-194.0f, 47.0f, -149.0f);
	D3DXVECTOR3 snowBoy2Position = D3DXVECTOR3(-174.0f, 25.0f, -159.0f);
    D3DXVECTOR3 cameraPosition   = D3DXVECTOR3(-194.0f, 35.0f, -129.0f);
	D3DXMATRIX matIniWorldSnowBoy;
	D3DXMATRIX matIniWorldCube;
	D3DXMATRIX matIniWorldSnowBoy2;
    D3DXMATRIX matIniWorldTerrain;

	D3DXVECTOR3 cubeX = D3DXVECTOR3(10.0f, 0.0f, 0.0f);
	D3DXVECTOR3 cubeY = D3DXVECTOR3(0.0f, 10.0f, 0.0f);
	D3DXVECTOR3 cubeZ = D3DXVECTOR3(0.0f, 0.0f, 10.0f);
	//�������ʼXYZ������
	D3DXVECTOR3 cubeXIni = D3DXVECTOR3(10.0f, 0.0f, 0.0f);
	D3DXVECTOR3 cubeYIni = D3DXVECTOR3(0.0f, 10.0f, 0.0f);
	D3DXVECTOR3 cubeZIni = D3DXVECTOR3(0.0f, 0.0f, 10.0f);

    const char *helpMessage = "W:ǰ�� \nS:���� \nA:���� \nD:���� \nSpace: ������ \n�������:�ӽ���ת \n�������:�ӽ���ת \n H: ����/�ر� ��ʾ��Ϣ";

};

