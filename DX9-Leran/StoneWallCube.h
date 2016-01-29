#ifndef _STONE_WALL_CUBE_H_
#define _STONE_WALL_CUBE_H_
#include <d3d9.h>
#include <d3dx9.h>
#include <string>

#include "d3dUtility.h"
#include "ObjectBase.h"

class StoneWallCube: public ObjectBase
{
public:
	StoneWallCube(IDirect3DDevice9 *d, 
		const std::string &nTexFileName, 
		const std::string &texFileName,
		float l = 1.0f):ObjectBase(d), textureFileName(texFileName), normalMapTextureFileName(nTexFileName), length(l){}

	void init() override
	{
		D3DXMatrixScaling(&matScale, length, length, length);
		loadTexture();
		copyDataToVertexBuffer();
		initialTangentVector();
		computeTangentsMatricesForEachVertex();
	}
	void draw(D3DXMATRIX *matWorld = nullptr, D3DXVECTOR3 *lightDirection = nullptr) override
	{

		D3DXVECTOR3 vVertToLightMS = -(*lightDirection); //��������ϵ�й��߷�����
		D3DXVECTOR3 vVertToLightTS; //��������ϵ�ж��㵽��Դ����
		// Transform the light's position into model-space
		D3DXMATRIX worldInverse;
		D3DXMatrixInverse(&worldInverse, NULL, matWorld);
		D3DXVec3TransformNormal(&vVertToLightMS, &vVertToLightMS, &worldInverse);

		Vertex *pVertices = NULL;
		pVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
		for (int i = 0; i < NUM_VERTICES; ++i)
		{
			//������һ��
			D3DXVec3Normalize(&vVertToLightMS, &vVertToLightMS);

			//������������������������������������ϵ�й�Դ����
			//                           Tangent           Binormal           Normal
			D3DXMATRIX invTangentMatrix(vTangents[i].x, vBiNormals[i].x, vNormals[i].x, 0.0f,
				vTangents[i].y, vBiNormals[i].y, vNormals[i].y, 0.0f,
				vTangents[i].z, vBiNormals[i].z, vNormals[i].z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);

			D3DXVec3TransformNormal(&vVertToLightTS, &vVertToLightMS, &invTangentMatrix);
			//������ת��Ϊ��ɫֵ
			pVertices[i].diffuse = encodeVectorAsDWORDColor(&vVertToLightTS, 0.77f);
		}
		pVertexBuffer->Unlock();

		auto world = matScale*(*matWorld);

        //��������任
		pDevice->SetTransform(D3DTS_WORLD, &world);

		pDevice->SetTexture(0, pNormalMapTexture);
		//pDevice->SetTexture(0, pTexture);
		pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
		pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DOTPRODUCT3); // Perform a Dot3 operation...
		pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);    // between the N (of N.L) which is stored in a normal map texture...
		pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);    // with the L (of N.L) which is stored in the vertex's diffuse color.

		pDevice->SetTexture(1, pTexture);
		//pDevice->SetTexture(1, NULL);
		pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
		pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE); // Modulate...
		pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE); // the texture for this stage with...
		pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT); // the current argument passed down from stage 0

		pDevice->SetRenderState(D3DRS_LIGHTING, false);

		pDevice->SetStreamSource(0, pVertexBuffer, 0, sizeof(Vertex));
		pDevice->SetFVF(Vertex::FVF_Flags);

		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, 2);
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 8, 2);
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 12, 2);
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 16, 2);
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 20, 2);

		pDevice->SetRenderState(D3DRS_LIGHTING, true);

		pDevice->SetTexture(0, NULL);
		pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE); // Perform a Dot3 operation...
		pDevice->SetTexture(1, NULL);

	}
	~StoneWallCube()
	{
		d3dUtil::Release(pNormalMapTexture);
		d3dUtil::Release(pTexture);
		d3dUtil::Release(pVertexBuffer);
	}

private:
	static const int NUM_VERTICES = 24;
	IDirect3DTexture9 *pNormalMapTexture;
	IDirect3DTexture9 *pTexture;
	IDirect3DVertexBuffer9 *pVertexBuffer;

	std::string normalMapTextureFileName;
	std::string textureFileName;

	float length;
	D3DXMATRIX matScale;

	D3DXVECTOR3 vTangents[NUM_VERTICES];
	D3DXVECTOR3 vBiNormals[NUM_VERTICES];
	D3DXVECTOR3 vNormals[NUM_VERTICES];


	void loadTexture()
	{
		//���뷨������
		auto hr = D3DXCreateTextureFromFile(pDevice, normalMapTextureFileName.c_str(), &pNormalMapTexture);

        if (FAILED(hr))
        {
            throw d3dUtil::ProjectError("load stone wall cube normal map texture failed.");
        }
		pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

		//����ͼ������
		hr = D3DXCreateTextureFromFile(pDevice, textureFileName.c_str(), &pTexture);
        if (FAILED(hr))
        {
            throw d3dUtil::ProjectError("load stone wall cube texture failed.");
        }

		pDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	}
	void copyDataToVertexBuffer()
	{
		auto hr = pDevice->CreateVertexBuffer(
			24 * sizeof(Vertex),
			D3DUSAGE_DYNAMIC,
			Vertex::FVF::FVF_Flags,
			D3DPOOL_DEFAULT,
			&pVertexBuffer,
			0);
		if (FAILED(hr))
		{
            throw d3dUtil::ProjectError("create vertex buffer failed.");
		}
		Vertex * pVertices = 0;
		pVertexBuffer->Lock(0, sizeof(cubeVertices), (void **)&pVertices, 0);
		memcpy(pVertices, cubeVertices, sizeof(cubeVertices));
		pVertexBuffer->Unlock();
	}
	void initialTangentVector()
	{
		for (int i = 0; i < 24; ++i)
		{
			vTangents[i] = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
			vBiNormals[i] = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			vNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
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

			//v1 v2 v3 ����ͼ
			v1 = D3DXVECTOR3(cubeVertices[i].x, cubeVertices[i].y, cubeVertices[i].z);
			v2 = D3DXVECTOR3(cubeVertices[i + 3].x, cubeVertices[i + 3].y, cubeVertices[i + 3].z);
			v3 = D3DXVECTOR3(cubeVertices[i + 1].x, cubeVertices[i + 1].y, cubeVertices[i + 1].z);

			d3dUtil::createTangentSpaceVectors(&v1, &v2, &v3,
				cubeVertices[i].tu1, cubeVertices[i].tv1,
				cubeVertices[i + 3].tu1, cubeVertices[i + 3].tv1,
				cubeVertices[i + 1].tu1, cubeVertices[i + 1].tv1,
				&vTangent, &vBiNormal, &vNormal);
			vTangents[i] = vTangent;
			vBiNormals[i] = vBiNormal;
			vNormals[i] = vNormal;

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

			v1 = D3DXVECTOR3(cubeVertices[i + 1].x, cubeVertices[i + 1].y, cubeVertices[i + 1].z);
			v2 = D3DXVECTOR3(cubeVertices[i].x, cubeVertices[i].y, cubeVertices[i].z);
			v3 = D3DXVECTOR3(cubeVertices[i + 2].x, cubeVertices[i + 2].y, cubeVertices[i + 2].z);

			d3dUtil::createTangentSpaceVectors(&v1, &v2, &v3,
				cubeVertices[i + 1].tu1, cubeVertices[i + 1].tv1,
				cubeVertices[i].tu1, cubeVertices[i].tv1,
				cubeVertices[i + 2].tu1, cubeVertices[i + 2].tv1,
				&vTangent, &vBiNormal, &vNormal);

			vTangents[i + 1] = vTangent;
			vBiNormals[i + 1] = vBiNormal;
			vNormals[i + 1] = vNormal;

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

			v1 = D3DXVECTOR3(cubeVertices[i + 2].x, cubeVertices[i + 2].y, cubeVertices[i + 2].z);
			v2 = D3DXVECTOR3(cubeVertices[i + 1].x, cubeVertices[i + 1].y, cubeVertices[i + 1].z);
			v3 = D3DXVECTOR3(cubeVertices[i + 3].x, cubeVertices[i + 3].y, cubeVertices[i + 3].z);

			d3dUtil::createTangentSpaceVectors(&v1, &v2, &v3,
				cubeVertices[i + 2].tu1, cubeVertices[i + 2].tv1,
				cubeVertices[i + 1].tu1, cubeVertices[i + 1].tv1,
				cubeVertices[i + 3].tu1, cubeVertices[i + 3].tv1,
				&vTangent, &vBiNormal, &vNormal);

			vTangents[i + 2] = vTangent;
			vBiNormals[i + 2] = vBiNormal;
			vNormals[i + 2] = vNormal;

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

			v1 = D3DXVECTOR3(cubeVertices[i + 3].x, cubeVertices[i + 3].y, cubeVertices[i + 3].z);
			v2 = D3DXVECTOR3(cubeVertices[i + 2].x, cubeVertices[i + 2].y, cubeVertices[i + 2].z);
			v3 = D3DXVECTOR3(cubeVertices[i].x, cubeVertices[i].y, cubeVertices[i].z);

			d3dUtil::createTangentSpaceVectors(&v1, &v2, &v3,
				cubeVertices[i + 3].tu1, cubeVertices[i + 3].tv1,
				cubeVertices[i + 2].tu1, cubeVertices[i + 2].tv1,
				cubeVertices[i].tu1, cubeVertices[i].tv1,
				&vTangent, &vBiNormal, &vNormal);

			vTangents[i + 3] = vTangent;
			vBiNormals[i + 3] = vBiNormal;
			vNormals[i + 3] = vNormal;
		}
	}
	DWORD encodeVectorAsDWORDColor(D3DXVECTOR3* vVector, float baseScale = 0.5f)
	{
		return D3DCOLOR_COLORVALUE(
			baseScale + (1.0f - baseScale)*vVector->x, 
			baseScale + (1.0f - baseScale)*vVector->y,
			baseScale + (1.0f - baseScale)*vVector->z,
			1.0f);
	}
	//�����ʽ
	struct Vertex
	{
		float x, y, z;
		DWORD diffuse;
		float tu1, tv1;
		float tu2, tv2;
		enum FVF
		{
			FVF_Flags = D3DFVF_XYZ  | D3DFVF_DIFFUSE | D3DFVF_TEX2
		};
	};
	Vertex cubeVertices[NUM_VERTICES] =
	{
		//     x     y     z                       r    g    b    a        tu1 tv1    tu2 tv2
		// Front Face
		{ -1.0f, 1.0f,-1.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,0.0f, 0.0f,0.0f, },
		{ 1.0f, 1.0f,-1.0f,   D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,0.0f, 1.0f,0.0f, },
		{ -1.0f,-1.0f,-1.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,1.0f, 0.0f,1.0f, },
		{ 1.0f,-1.0f,-1.0f,   D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,1.0f, 1.0f,1.0f, },
		// Back Face
		{ -1.0f, 1.0f, 1.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,0.0f, 1.0f,0.0f, },
		{ -1.0f,-1.0f, 1.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,1.0f, 1.0f,1.0f, },
		{ 1.0f, 1.0f, 1.0f,   D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,0.0f, 0.0f,0.0f, },
		{ 1.0f,-1.0f, 1.0f,   D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,1.0f, 0.0f,1.0f, },
		// Top Face
		{ -1.0f, 1.0f, 1.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,0.0f, 0.0f,0.0f, },
		{ 1.0f, 1.0f, 1.0f,   D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,0.0f, 1.0f,0.0f, },
		{ -1.0f, 1.0f,-1.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,1.0f, 0.0f,1.0f, },
		{ 1.0f, 1.0f,-1.0f,   D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,1.0f, 1.0f,1.0f, },
		// Bottom Face
		{ -1.0f,-1.0f, 1.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,1.0f, 0.0f,1.0f, },
		{ -1.0f,-1.0f,-1.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,0.0f, 0.0f,0.0f, },
		{ 1.0f,-1.0f, 1.0f,   D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,1.0f, 1.0f,1.0f, },
		{ 1.0f,-1.0f,-1.0f,   D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,0.0f, 1.0f,0.0f, },
		// Right Face
		{ 1.0f, 1.0f,-1.0f,   D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,0.0f, 0.0f,0.0f, },
		{ 1.0f, 1.0f, 1.0f,   D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,0.0f, 1.0f,0.0f, },
		{ 1.0f,-1.0f,-1.0f,   D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,1.0f, 0.0f,1.0f, },
		{ 1.0f,-1.0f, 1.0f,   D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,1.0f, 1.0f,1.0f, },
		// Left Face
		{ -1.0f, 1.0f,-1.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,0.0f, 1.0f,0.0f, },
		{ -1.0f,-1.0f,-1.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   1.0f,1.0f, 1.0f,1.0f, },
		{ -1.0f, 1.0f, 1.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,0.0f, 0.0f,0.0f, },
		{ -1.0f,-1.0f, 1.0f,  D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f),   0.0f,1.0f, 0.0f,1.0f, }
	};

};

#endif // !_STONE_WALL_CUBE_H_