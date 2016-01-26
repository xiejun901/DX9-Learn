#include <d3d9.h>
#include <d3dx9.h>

#include "SnowBoy.h"

D3DXMATRIX SnowBoy::headMatTrans(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 2.5f, 0.0f, 1.0f
    );

void SnowBoy::init()
{
	body = createTextureSphere(2.0f, 40, 40);
	head = createTextureSphere(1.0f, 40, 40);
	loadTexture();
}

void SnowBoy::draw(D3DXMATRIX * matWorld, D3DXVECTOR3 * lightDirection)
{
	auto directionToLight = -*lightDirection;
	D3DXMATRIX worldInverse;
	D3DXMatrixInverse(&worldInverse, NULL, matWorld);
	D3DXVec3TransformNormal(&directionToLight, &directionToLight, &worldInverse);
	lightSnowBoy(directionToLight);
	pDevice->SetRenderState(D3DRS_LIGHTING, false);
	pDevice->SetTexture(0, bodyTexture);
	pDevice->SetTransform(D3DTS_WORLD, matWorld);
	body->DrawSubset(0);

	pDevice->SetTexture(0, headTexture);
	auto matWorldHead = headMatTrans*(*matWorld);
	pDevice->SetTransform(D3DTS_WORLD, &matWorldHead);
	head->DrawSubset(0);
	pDevice->SetRenderState(D3DRS_LIGHTING, true);
}

SnowBoy::~SnowBoy()
{
	d3dUtil::Release(body);
	d3dUtil::Release(head);
}

void SnowBoy::loadTexture()
{
	D3DXCreateTextureFromFile(
		pDevice,
		headTextureFileName.c_str(),
		&headTexture);
	D3DXCreateTextureFromFile(
		pDevice,
		bodyTextureFileName.c_str(),
		&bodyTexture);
}

ID3DXMesh * SnowBoy::createTextureSphere(float radius, UINT slices, UINT stacks)
{
	ID3DXMesh *mesh;
	auto hr = D3DXCreateSphere(
		pDevice,
		radius,
		slices,
		stacks,
		&mesh,
		0);
	if (FAILED(hr))
	{
		MessageBox(0, "create sphere failed", 0, 0);
		return nullptr;
	}
	ID3DXMesh *texMesh = nullptr;
	mesh->CloneMeshFVF(
		D3DXMESH_SYSTEMMEM,
		Vertex::FVF::FVF_Flags,
		pDevice,
		&texMesh);
	d3dUtil::Release(mesh);

	Vertex *pVertices = nullptr;
	hr = texMesh->LockVertexBuffer(0, (void **)&pVertices);
	auto numVertices = texMesh->GetNumVertices();
	for (size_t i = 0; i < numVertices; ++i)
	{
		float u = (-asinf(pVertices->nx) / D3DX_PI + 0.5f) / 2;
		float v = (-asinf(pVertices->ny) / D3DX_PI + 0.5f) / 2;
		if (pVertices->nz <= 0)
		{
			pVertices->u = u;
			pVertices->v = v;
		}
		else
		{
			pVertices->u = u + 0.5f;
			pVertices->v = v + 0.5f;
		}

		pVertices->diffuse = D3DCOLOR_COLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);
		++pVertices;
	}
	texMesh->UnlockVertexBuffer();
	return texMesh;
}

void SnowBoy::light(ID3DXMesh * mesh, D3DXVECTOR3 directionToLight)
{
	D3DXVec3Normalize(&directionToLight, &directionToLight);
	Vertex *pVertices = nullptr;
	auto hr = mesh->LockVertexBuffer(0, (void **)&pVertices);
	auto numVertices = mesh->GetNumVertices();
	for (size_t i = 0; i < numVertices; ++i)
	{
		D3DXVECTOR3 normal(pVertices->nx, pVertices->ny, pVertices->nz);
		D3DXVec3Normalize(&normal, &normal);
		auto cosine = D3DXVec3Dot(&normal, &directionToLight);

		//pVertices->diffuse = pVertices->diffuse*0.5f + pVertices->diffuse*cosine*0.5;
		float baseScale = 0.57f;
		pVertices->diffuse = D3DCOLOR_COLORVALUE(baseScale + (1.0f - baseScale)*cosine, baseScale + (1.0f - baseScale)*cosine, baseScale + (1.0f - baseScale)*cosine, 1.0f);
		++pVertices;
	}
	mesh->UnlockVertexBuffer();
}

void SnowBoy::lightSnowBoy(const D3DXVECTOR3 & directionToLigh)
{
	light(head, directionToLigh);
	light(body, directionToLigh);
}
