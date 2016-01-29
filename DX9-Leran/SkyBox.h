#ifndef _SKY_BOX_H_
#define _SKY_BOX_H_
#include<d3d9.h>
#include<d3dx9.h>
#include<string>

#include "ObjectBase.h"
#include "d3dUtility.h"
class SkyBox:public ObjectBase
{
public:
	SkyBox(IDirect3DDevice9 *d, 
		const std::string &textureUp = "skybox_up.tga",
		const std::string &textureDown = "skybox_dn.tga",
		const std::string &textureLeft = "skybox_lf.tga",
		const std::string &textureRight = "skybox_rt.tga",
		const std::string &textureFront = "skybox_ft.tga",
		const std::string &textureBack = "skybox_bk.tga"
		):ObjectBase(d),
		textureFileName{ textureBack,textureFront , textureDown,  textureUp, textureLeft , textureRight }
	{

		//pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	}
	void init() override
	{
		auto lenght = 2000.0f;
		auto hr = pDevice->CreateVertexBuffer(
			24 * sizeof(Vertex),
			D3DUSAGE_WRITEONLY,
			Vertex::FVF,
			D3DPOOL_MANAGED,
			&vb,
			0);
        if (FAILED(hr))
        {
            throw d3dUtil::ProjectError("create sky box vertex buffer failed");
        }
		Vertex *v;
		vb->Lock(0, sizeof(g_cubeVertices), (void **)&v, 0);
		memcpy(v, g_cubeVertices, sizeof(g_cubeVertices));
		vb->Unlock();
		
		//load texture
		loadTexture();

	}
	void draw(D3DXMATRIX *matWorld = nullptr, D3DXVECTOR3 *lightDirection = nullptr) override
	{
		D3DXMATRIX world;
		D3DXMatrixScaling(
			&world,
			640,
			640,
			640);
		D3DXMATRIX tranWorld;
		D3DXMatrixTranslation(
			&tranWorld,
			0,
			150,
			0);
		world = world * tranWorld *(*matWorld);
		pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		pDevice->SetTransform(D3DTS_WORLD, &world);
		pDevice->SetStreamSource(0, vb, 0, sizeof(Vertex));
		pDevice->SetFVF(Vertex::FVF);
		for (int i = 0; i < 6; ++i)
		{
			pDevice->SetRenderState(D3DRS_LIGHTING, false);
			pDevice->SetTexture(0, tex[i]);
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4*i, 2);
		}
		pDevice->SetRenderState(D3DRS_LIGHTING, true);
	}
	void loadTexture()
	{
		for (int i = 0; i < 6; i++)
		{
			auto hr = D3DXCreateTextureFromFile(
				pDevice,
				textureFileName[i].c_str(),
				&tex[i]);
			if (FAILED(hr))
			{
				//MessageBox(0, "create up texture failed.", 0, 0);
				//return;
                throw d3dUtil::ProjectError("create sky box texture failed");
			}
		}
	}
	~SkyBox()
	{
		d3dUtil::Release(vb);
		for (int i = 0; i < 6; i++)
			d3dUtil::Release(tex[i]);
	}


private:
	IDirect3DVertexBuffer9 *vb;
	IDirect3DTexture9 *tex[6];// back front down up left right
	std::string textureFileName[6];

	struct Vertex
	{
		Vertex() = default;
		Vertex(float x, float y, float z,
			float u, float v) :_x(x), _y(y), _z(z) , _u(u), _v(v) {}
		float _x, _y, _z;
		float _u, _v;
		static const DWORD FVF;
	};
	Vertex g_cubeVertices[24] =
	{
		//后面
		{ -1.0f, 1.0f,1.0f,  0.0f,0.0f },
		{ 1.0f, 1.0f,1.0f,  1.0f,0.0f },
		{ -1.0f,-1.0f,1.0f,  0.0f,1.0f },
		{ 1.0f,-1.0f,1.0f,  1.0f,1.0f },

		//正面
		{ -1.0f, 1.0f, -1.0f,  1.0f,0.0f },
		{ -1.0f,-1.0f, -1.0f,  1.0f,1.0f },
		{ 1.0f, 1.0f, -1.0f,  0.0f,0.0f },
		{ 1.0f,-1.0f, -1.0f,  0.0f,1.0f },

		//下面
		{ -1.0f, -1.0f, 1.0f,  0.0f,0.0f },
		{ 1.0f, -1.0f, 1.0f,  1.0f,0.0f },
		{ -1.0f, -1.0f,-1.0f,  0.0f,1.0f },
		{ 1.0f, -1.0f,-1.0f,  1.0f,1.0f },

		//上面
		{ -1.0f,1.0f, 1.0f,  0.0f,0.0f },
		{ -1.0f,1.0f,-1.0f,  1.0f,0.0f },
		{ 1.0f,1.0f, 1.0f,  0.0f,1.0f },
		{ 1.0f,1.0f,-1.0f,  1.0f,1.0f },

		//左面
		{ -1.0f, 1.0f,-1.0f,  0.0f,0.0f },
		{ -1.0f, 1.0f, 1.0f,  1.0f,0.0f },
		{ -1.0f,-1.0f,-1.0f,  0.0f,1.0f },
		{ -1.0f,-1.0f, 1.0f,  1.0f,1.0f },

		//右面
		{ 1.0f, 1.0f,-1.0f,  1.0f,0.0f },
		{ 1.0f,-1.0f,-1.0f,  1.0f,1.0f },
		{ 1.0f, 1.0f, 1.0f,  0.0f,0.0f },
		{ 1.0f,-1.0f, 1.0f,  0.0f,1.0f }
	};
};

#endif

