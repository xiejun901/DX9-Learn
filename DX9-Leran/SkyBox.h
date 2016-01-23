#ifndef _SKY_BOX_H_
#define _SKY_BOX_H_
#include<d3d9.h>
#include<d3dx9.h>
#include<string>
class SkyBox
{
public:
	SkyBox(IDirect3DDevice9 *d):device(d)
	{
		auto lenght = 2000.0f;
		const Vertex g_cubeVertices[] =
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
		loadTexture("crate.jpg");
		device->CreateVertexBuffer(
			24 * sizeof(Vertex),
			D3DUSAGE_WRITEONLY,
			Vertex::FVF,
			D3DPOOL_MANAGED,
			&vb,
			0);

		Vertex *v;
		vb->Lock(0, sizeof(g_cubeVertices), (void **)&v, 0);
		memcpy(v, g_cubeVertices, sizeof(g_cubeVertices));
		vb->Unlock();
		device->SetRenderState(D3DRS_LIGHTING, FALSE);
		device->SetRenderState(D3DRS_ZENABLE, TRUE);
	}

	void loadTexture(const std::string &fileName)
	{
		auto hr = D3DXCreateTextureFromFile(
			device,
			fileName.c_str(),
			&tex);
	}
	void loadUpTexture(const std::string &fileName)
	{
		auto hr = D3DXCreateTextureFromFile(
			device,
			fileName.c_str(),
			&texUp);
		if (FAILED(hr))
		{
			MessageBox(0, "create up texture failed.", 0, 0);
			return;
		}
	}
	void loadDownTexture(const std::string &fileName)
	{
		auto hr = D3DXCreateTextureFromFile(
			device,
			fileName.c_str(),
			&texDn);
		if (FAILED(hr))
		{
			MessageBox(0, "create down texture failed.", 0, 0);
			return;
		}
	}
	void loadLeftTexture(const std::string &fileName)
	{
		auto hr = D3DXCreateTextureFromFile(
			device,
			fileName.c_str(),
			&texLf);
		if (FAILED(hr))
		{
			MessageBox(0, "create left texture failed.", 0, 0);
			return;
		}
	}
	void loadRightTexture(const std::string &fileName)
	{
		auto hr = D3DXCreateTextureFromFile(
			device,
			fileName.c_str(),
			&texRt);
		if (FAILED(hr))
		{
			MessageBox(0, "create right texture failed.", 0, 0);
			return;
		}
	}
	void loadFrontTexture(const std::string &fileName)
	{
		auto hr = D3DXCreateTextureFromFile(
			device,
			fileName.c_str(),
			&texFt);
		if (FAILED(hr))
		{
			MessageBox(0, "create front texture failed.", 0, 0);
			return;
		}
	}
	void loadBackTexture(const std::string &fileName)
	{
		auto hr = D3DXCreateTextureFromFile(
			device,
			fileName.c_str(),
			&texBk);
		if (FAILED(hr))
		{
			MessageBox(0, "create backup texture failed.", 0, 0);
			return;
		}
	}
	void draw()
	{
		D3DXMATRIX word;
		D3DXMatrixScaling(
			&word,
			640,
			640,
			640);
		D3DXMATRIX tranWord;
		D3DXMatrixTranslation(
			&tranWord,
			0,
			150,
			0);
		D3DXMatrixMultiply(&word, &word, &tranWord);
		device->SetTransform(D3DTS_WORLD, &word);
		device->SetStreamSource(0, vb, 0, sizeof(Vertex));
		device->SetFVF(Vertex::FVF);
		device->SetRenderState(D3DRS_LIGHTING, false);
		device->SetTexture(0, texBk);
		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		device->SetTexture(0, texFt);
		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, 2);
		device->SetTexture(0, texDn);
		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 8, 2);
		device->SetTexture(0, texUp);
		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 12, 2);
		device->SetTexture(0, texLf);
		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 16, 2);
		device->SetTexture(0, texRt);
		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 20, 2);
		device->SetRenderState(D3DRS_LIGHTING, true);
	}
	~SkyBox();


private:
	IDirect3DDevice9 *device;
	IDirect3DVertexBuffer9 *vb;
	IDirect3DTexture9 *tex;
	IDirect3DTexture9 *texUp, *texDn, *texLf, *texRt, *texFt, *texBk;

	struct Vertex
	{
		Vertex() = default;
		Vertex(float x, float y, float z,
			float u, float v) :_x(x), _y(y), _z(z) , _u(u), _v(v) {}
		float _x, _y, _z;
		float _u, _v;
		static const DWORD FVF;
	};
};

#endif

