#pragma once
#include "d3dMainWindow.h"
#include "Camera.h"
#include "d3dUtility.h"
//顶点信息

class Dot3CubeWindow :
	public D3DMainWindow
{
public:
	Dot3CubeWindow();
	Dot3CubeWindow(const char *n, int w, int h): D3DMainWindow(n,w,h){}
	void Setup() override
	{
		//载入纹理
		loadTexture();
		//复制顶点数据到顶点缓存
		copyDataToVertexBuffer();
		//设置视口变换
		D3DXMATRIX matProj;
		D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(45.0f),
			float(width) / float(height), 0.1f, 100.0f);
		pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

		pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		//g_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT ); // Direct3d doesn't like this at all when doing Dot3.
		pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

		//设置光照
		pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

		//初始化切向向量，法向量，副法向量
		initial3Vector();
		//计算每个顶点的三个向量， 切向量，法向量，副法向量
		computeTangentsMatricesForEachVertex();




	}
	void Cleanup() override
	{

	}
	bool Display(float timeDelta) override
	{

		unsigned char keys[256];
		GetKeyboardState(keys);

		// key W is Down
		if (keys['W'] & 0x80)
			camera.walk(10.0f*timeDelta);

		//key S is Down
		if (keys['S'] & 0x80)
			camera.walk(-10.0f*timeDelta);

		//key A is Down
		if (keys['A'] & 0x80)
			camera.strafe(-10.0f * timeDelta);
		//key D is Down
		if (keys['D'] & 0x80)
			camera.strafe(10.0f * timeDelta);

		if (keys[VK_LEFT] & 0x80)
			camera.yaw(-timeDelta);

		if (keys[VK_RIGHT] & 0x80)
			camera.yaw(timeDelta);

		if (keys[VK_UP] & 0x80)
			camera.pitch(-timeDelta);

		if (keys[VK_DOWN] & 0x80)
			camera.pitch(timeDelta);

		if (keys['Q'] & 0x80)
			camera.fly(10.0f * timeDelta);

		if (keys['E'] & 0x80)
			camera.fly(-10.0f * timeDelta);


		//清屏
		pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			D3DCOLOR_COLORVALUE(0.35f, 0.53f, 0.7, 1.0f), 1.0f, 0);

		//对立方体作世界变换到适合观察的位置
		D3DXMATRIX matWorld;
		D3DXMATRIX matView;
		D3DXMATRIX matProj;
		D3DXMATRIX matTrans;
		D3DXMATRIX matRot;

		static float fAngle = 0;
		fAngle += 60 * timeDelta;
		// Wrap it around, if it gets too big
		while (fAngle > 360.0f) fAngle -= 360.0f;
		while (fAngle < 0.0f)   fAngle += 360.0f;

		//平移矩阵
		D3DXMatrixTranslation(&matTrans, 0.0f, 0.0f, 10.0f);
		//旋转矩阵
		D3DXMatrixRotationYawPitchRoll(&matRot,
			D3DXToRadian(fAngle),
			D3DXToRadian(0.0f),
			0.0f);
		matWorld = matRot * matTrans;
		pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

		//开始画图
		pd3dDevice->BeginScene();

		//法线贴图求各顶点点颜色
		
		D3DXVECTOR3 vLightPosWS;    // Light position (in world-space)
		D3DXVECTOR3 vLightPosMS;    // Light position (in model-space)
		D3DXVECTOR3 vVertToLightMS; // L vector of N.L (in model-space)
		D3DXVECTOR3 vVertToLightTS; // L vector of N.L (in tangent-space)


		vLightPosWS = D3DXVECTOR3(1000000.0f, 0.0f, 5.0f);
		// Transform the light's position into model-space
		D3DXMATRIX worldInverse;
		D3DXMatrixInverse(&worldInverse, NULL, &matWorld);
		D3DXVec3TransformCoord(&vLightPosMS, &vLightPosWS, &worldInverse);
		//
		// Since our cube's vertex data is stored in a Vertex Buffer, we will
		// need to lock it briefly so we can encode the new tangent-space
		// L vectors that we're going to create into the diffuse color of each 
		// vertex.
		//

		Vertex *pVertices = NULL;
		g_pVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);

		for (int i = 0; i < NUM_VERTICES; ++i)
		{
			//
			// For each vertex, rotate L (of N.L) into tangent-space and 
			// pass it into Direct3D's texture blending system by packing it 
			// into the vertex's diffuse color.
			//

			D3DXVECTOR3 vCurrentVertex = D3DXVECTOR3(g_cubeVertices[i].x,
				g_cubeVertices[i].y,
				g_cubeVertices[i].z);

			//光线位置减去当前点的位置，这个是指向光源的方向， 这个地方跟计算地形的着色那个是一样的
			vVertToLightMS = vLightPosMS - vCurrentVertex;
			//向量归一化
			D3DXVec3Normalize(&vVertToLightMS, &vVertToLightMS);

			//
			// Build up an inverse tangent-space matrix using the Tangent, 
			// Binormal, and Normal calculated for the current vertex and 
			// then use it to transform our L vector (of N.L), which is in 
			// model-space, into tangent-space.
			//
			// A tangent matrix is of the form:
			//
			// |Tx Bx Nx 0|
			// |Ty By Ny 0|
			// |Tz Bz Nz 0|
			// |0  0  0  1|
			//
			// Note: Our vectors have already been inverted, so there is no 
			// need to invert our tangent matrix once we build it up.
			//

			//利用切向量，法向量，副法向量求
			//                           Tangent           Binormal           Normal
			D3DXMATRIX invTangentMatrix(g_vTangents[i].x, g_vBiNormals[i].x, g_vNormals[i].x, 0.0f,
				g_vTangents[i].y, g_vBiNormals[i].y, g_vNormals[i].y, 0.0f,
				g_vTangents[i].z, g_vBiNormals[i].z, g_vNormals[i].z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);

			D3DXVec3TransformNormal(&vVertToLightTS, &vVertToLightMS, &invTangentMatrix);

			//
			// Last but not least, we must encode our new L vector as a DWORD 
			// value so we can set it as the new vertex color. Of course, the 
			// hardware assumes that we are  going to do this, so it will 
			// simply decode the original vector back out by reversing the 
			// DOWRD encdoing we've performed here.
			//

			pVertices[i].diffuse = encodeVectorAsDWORDColor(&vVertToLightTS);
		}

		g_pVertexBuffer->Unlock();

		//
		// STAGE 0
		//
		// Use D3DTOP_DOTPRODUCT3 to find the dot-product of (N.L), where N is 
		// stored in the normal map, and L is passed in as the vertex color - 
		// D3DTA_DIFFUSE.
		//

		pd3dDevice->SetTexture(0, g_pNormalMapTexture);
		pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);

		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DOTPRODUCT3); // Perform a Dot3 operation...
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);    // between the N (of N.L) which is stored in a normal map texture...
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);    // with the L (of N.L) which is stored in the vertex's diffuse color.

																				   //
																				   // STAGE 1
																				   //
																				   // Modulate the base texture by N.L calculated in STAGE 0.
																				   //

		pd3dDevice->SetTexture(1, g_pTexture);
		pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);

		pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE); // Modulate...
		pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE); // the texture for this stage with...
		pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT); // the current argument passed down from stage 0

		pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

		pd3dDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(Vertex));
		pd3dDevice->SetFVF(Vertex::FVF_Flags);

		pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, 2);
		pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 8, 2);
		pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 12, 2);
		pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 16, 2);
		pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 20, 2);

		//
		// Render a small white sphere to mark the point light's position...
		//



		D3DXMATRIX V;
		camera.getViewMatrix(&V);
		pd3dDevice->SetTransform(D3DTS_VIEW, &V);

		pd3dDevice->EndScene();
		pd3dDevice->Present(NULL, NULL, NULL, NULL);

		return true;
	}
	~Dot3CubeWindow();



	DWORD encodeVectorAsDWORDColor(D3DXVECTOR3* vVector)
	{
		DWORD dwRed = (DWORD)(127 * vVector->x + 128);
		DWORD dwGreen = (DWORD)(127 * vVector->y + 128);
		DWORD dwBlue = (DWORD)(127 * vVector->z + 128);

		return (DWORD)(0xff000000 + (dwRed << 16) + (dwGreen << 8) + dwBlue);
	}

	void loadTexture(void)
	{
		
		//载入法向纹理
		D3DXCreateTextureFromFile(pd3dDevice, "stone_wall_normal_map.bmp", &g_pNormalMapTexture);
		//D3DXCreateTextureFromFile( g_pd3dDevice, "test_normal_map.bmp", &g_pNormalMapTexture );

		pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

		//载入图像纹理
		D3DXCreateTextureFromFile(pd3dDevice, "stone_wall.bmp", &g_pTexture);
		//D3DXCreateTextureFromFile( g_pd3dDevice, "checker_with_numbers.bmp", &g_pTexture );

		pd3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	}

	void copyDataToVertexBuffer()
	{
		pd3dDevice->CreateVertexBuffer(
			24 * sizeof(Vertex),
			D3DUSAGE_DYNAMIC,
			Vertex::FVF::FVF_Flags,
			D3DPOOL_DEFAULT,
			&g_pVertexBuffer,
			0);
		Vertex * pVertices = 0;
		g_pVertexBuffer->Lock(0, sizeof(g_cubeVertices), (void **)&pVertices, 0);
		memcpy(pVertices, g_cubeVertices, sizeof(g_cubeVertices));
		g_pVertexBuffer->Unlock();
	}


	void initial3Vector()
	{
		for (int i = 0; i < 24; ++i)
		{
			g_vTangents[i] = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
			g_vBiNormals[i] = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			g_vNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		}
	}

	void computeTangentsMatricesForEachVertex(void)
	{
		D3DXVECTOR3 v1;
		D3DXVECTOR3 v2;
		D3DXVECTOR3 v3;
		D3DXVECTOR3 vTangent;
		D3DXVECTOR3 vBiNormal;
		D3DXVECTOR3 vNormal;

		//
		// For each cube face defined in the vertex array, compute a tangent matrix 
		// for each of the four vertices that define it.
		//

		for (int i = 0; i < NUM_VERTICES; i += 4) // Use += 4 to process 1 face at a time
		{
			//
			// Vertex 0 of current cube face...
			//
			//  v2
			//    3----2
			//    |    |
			//    |    |
			//    0----1
			//  v1      v3
			//

			//v1 v2 v3 如上图
			v1 = D3DXVECTOR3(g_cubeVertices[i].x, g_cubeVertices[i].y, g_cubeVertices[i].z);
			v2 = D3DXVECTOR3(g_cubeVertices[i + 3].x, g_cubeVertices[i + 3].y, g_cubeVertices[i + 3].z);
			v3 = D3DXVECTOR3(g_cubeVertices[i + 1].x, g_cubeVertices[i + 1].y, g_cubeVertices[i + 1].z);

			d3dUtil::createTangentSpaceVectors(&v1, &v2, &v3,
				g_cubeVertices[i].tu1, g_cubeVertices[i].tv1,
				g_cubeVertices[i + 3].tu1, g_cubeVertices[i + 3].tv1,
				g_cubeVertices[i + 1].tu1, g_cubeVertices[i + 1].tv1,
				&vTangent, &vBiNormal, &vNormal);
			g_vTangents[i] = vTangent;
			g_vBiNormals[i] = vBiNormal;
			g_vNormals[i] = vNormal;

			//
			// Vertex 1 of current cube face...
			//
			//          v3
			//    3----2
			//    |    |
			//    |    |
			//    0----1
			//  v2      v1
			//

			v1 = D3DXVECTOR3(g_cubeVertices[i + 1].x, g_cubeVertices[i + 1].y, g_cubeVertices[i + 1].z);
			v2 = D3DXVECTOR3(g_cubeVertices[i].x, g_cubeVertices[i].y, g_cubeVertices[i].z);
			v3 = D3DXVECTOR3(g_cubeVertices[i + 2].x, g_cubeVertices[i + 2].y, g_cubeVertices[i + 2].z);

			d3dUtil::createTangentSpaceVectors(&v1, &v2, &v3,
				g_cubeVertices[i + 1].tu1, g_cubeVertices[i + 1].tv1,
				g_cubeVertices[i].tu1, g_cubeVertices[i].tv1,
				g_cubeVertices[i + 2].tu1, g_cubeVertices[i + 2].tv1,
				&vTangent, &vBiNormal, &vNormal);

			g_vTangents[i + 1] = vTangent;
			g_vBiNormals[i + 1] = vBiNormal;
			g_vNormals[i + 1] = vNormal;

			//
			// Vertex 2 of current cube face...
			//
			//  v3      v1
			//    3----2
			//    |    |
			//    |    |
			//    0----1
			//          v2
			//

			v1 = D3DXVECTOR3(g_cubeVertices[i + 2].x, g_cubeVertices[i + 2].y, g_cubeVertices[i + 2].z);
			v2 = D3DXVECTOR3(g_cubeVertices[i + 1].x, g_cubeVertices[i + 1].y, g_cubeVertices[i + 1].z);
			v3 = D3DXVECTOR3(g_cubeVertices[i + 3].x, g_cubeVertices[i + 3].y, g_cubeVertices[i + 3].z);

			d3dUtil::createTangentSpaceVectors(&v1, &v2, &v3,
				g_cubeVertices[i + 2].tu1, g_cubeVertices[i + 2].tv1,
				g_cubeVertices[i + 1].tu1, g_cubeVertices[i + 1].tv1,
				g_cubeVertices[i + 3].tu1, g_cubeVertices[i + 3].tv1,
				&vTangent, &vBiNormal, &vNormal);

			g_vTangents[i + 2] = vTangent;
			g_vBiNormals[i + 2] = vBiNormal;
			g_vNormals[i + 2] = vNormal;

			//
			// Vertex 3 of current cube face...
			//
			//  v1      v2
			//    3----2
			//    |    |
			//    |    |
			//    0----1
			//  v3
			//

			v1 = D3DXVECTOR3(g_cubeVertices[i + 3].x, g_cubeVertices[i + 3].y, g_cubeVertices[i + 3].z);
			v2 = D3DXVECTOR3(g_cubeVertices[i + 2].x, g_cubeVertices[i + 2].y, g_cubeVertices[i + 2].z);
			v3 = D3DXVECTOR3(g_cubeVertices[i].x, g_cubeVertices[i].y, g_cubeVertices[i].z);

			d3dUtil::createTangentSpaceVectors(&v1, &v2, &v3,
				g_cubeVertices[i + 3].tu1, g_cubeVertices[i + 3].tv1,
				g_cubeVertices[i + 2].tu1, g_cubeVertices[i + 2].tv1,
				g_cubeVertices[i].tu1, g_cubeVertices[i].tv1,
				&vTangent, &vBiNormal, &vNormal);

			g_vTangents[i + 3] = vTangent;
			g_vBiNormals[i + 3] = vBiNormal;
			g_vNormals[i + 3] = vNormal;
		}
	}


	LPDIRECT3DTEXTURE9 g_pTexture = NULL;
	LPDIRECT3DTEXTURE9 g_pNormalMapTexture = NULL;
	LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;

	D3DXVECTOR3 g_vTangents[24];
	D3DXVECTOR3 g_vBiNormals[24];
	D3DXVECTOR3 g_vNormals[24];

	Camera camera;

	static const int NUM_VERTICES = 24;

	//顶点格式
	struct Vertex
	{
		float x, y, z;
		float nx, ny, nz;
		DWORD diffuse;
		float tu1, tv1;
		float tu2, tv2;

		enum FVF
		{
			FVF_Flags = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2
		};
	};

	Vertex g_cubeVertices[24] =
	{
		//     x     y     z      nx    ny    nz                         r    g    b    a        tu1 tv1    tu2 tv2
		// Front Face
		{ -1.0f, 1.0f,-1.0f,  0.0f, 0.0f,-1.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,0.0f, 0.0f,0.0f, },
		{ 1.0f, 1.0f,-1.0f,  0.0f, 0.0f,-1.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,0.0f, 1.0f,0.0f, },
		{ -1.0f,-1.0f,-1.0f,  0.0f, 0.0f,-1.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,1.0f, 0.0f,1.0f, },
		{ 1.0f,-1.0f,-1.0f,  0.0f, 0.0f,-1.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,1.0f, 1.0f,1.0f, },
		// Back Face
		{ -1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,0.0f, 1.0f,0.0f, },
		{ -1.0f,-1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,1.0f, 1.0f,1.0f, },
		{ 1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,0.0f, 0.0f,0.0f, },
		{ 1.0f,-1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,1.0f, 0.0f,1.0f, },
		// Top Face
		{ -1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,0.0f, 0.0f,0.0f, },
		{ 1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,0.0f, 1.0f,0.0f, },
		{ -1.0f, 1.0f,-1.0f,  0.0f, 1.0f, 0.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,1.0f, 0.0f,1.0f, },
		{ 1.0f, 1.0f,-1.0f,  0.0f, 1.0f, 0.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,1.0f, 1.0f,1.0f, },
		// Bottom Face
		{ -1.0f,-1.0f, 1.0f,  0.0f,-1.0f, 0.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,1.0f, 0.0f,1.0f, },
		{ -1.0f,-1.0f,-1.0f,  0.0f,-1.0f, 0.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,0.0f, 0.0f,0.0f, },
		{ 1.0f,-1.0f, 1.0f,  0.0f,-1.0f, 0.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,1.0f, 1.0f,1.0f, },
		{ 1.0f,-1.0f,-1.0f,  0.0f,-1.0f, 0.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,0.0f, 1.0f,0.0f, },
		// Right Face
		{ 1.0f, 1.0f,-1.0f,  1.0f, 0.0f, 0.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,0.0f, 0.0f,0.0f, },
		{ 1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,0.0f, 1.0f,0.0f, },
		{ 1.0f,-1.0f,-1.0f,  1.0f, 0.0f, 0.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,1.0f, 0.0f,1.0f, },
		{ 1.0f,-1.0f, 1.0f,  1.0f, 0.0f, 0.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,1.0f, 1.0f,1.0f, },
		// Left Face
		{ -1.0f, 1.0f,-1.0f, -1.0f, 0.0f, 0.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,0.0f, 1.0f,0.0f, },
		{ -1.0f,-1.0f,-1.0f, -1.0f, 0.0f, 0.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,1.0f, 1.0f,1.0f, },
		{ -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,0.0f, 0.0f,0.0f, },
		{ -1.0f,-1.0f, 1.0f, -1.0f, 0.0f, 0.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,1.0f, 0.0f,1.0f, }
	};

};

