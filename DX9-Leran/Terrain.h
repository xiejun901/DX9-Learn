#ifndef _TERRAIN_H_
#define _TERRAIN_H_
#include<d3d9.h>
#include<d3dx9.h>
#include<string>
#include<vector>
#include<fstream>

#include "d3dUtility.h"
class Terrain
{
public:
	Terrain(IDirect3DDevice9 * d, 
		std::string heightmapFileName,
		int _numVertsPerRow,
		int _numVertsPerCol,
		int _cellSpacing,
		float _heightScale):
		numVertsPerRow(_numVertsPerRow),
		numVertsPerCol(_numVertsPerCol),
		cellSpacing(_cellSpacing),
		heightScale(_heightScale),
		device(d)
	{
		numCellsPerCol = numVertsPerCol - 1;
		numCellsPerRow = numVertsPerRow - 1;
		width = numCellsPerRow * cellSpacing;
		depth = numCellsPerCol * cellSpacing;

		numVertices = numVertsPerRow * numVertsPerCol;
		numTriangles = numCellsPerRow * numCellsPerCol * 2;

		readHeightMapRawFile(heightmapFileName);
		caculateVertices();
		caculateIndices();

	}

	~Terrain();

	void draw(D3DXMATRIX* world)
	{
		if (device)
		{
			device->SetTransform(D3DTS_WORLD, world);

			device->SetStreamSource(0, vb, 0, sizeof(TerrainVertex));
			device->SetFVF(TerrainVertex::FVF);
			device->SetIndices(ib);
			device->SetTexture(0, tex);

			D3DLIGHT9 light;
			light.Type = D3DLIGHT_DIRECTIONAL;
			light.Ambient = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
			light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			light.Specular = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
			light.Direction = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
			device->SetLight(0, &light);
			device->LightEnable(0, true);

			D3DXCOLOR white(D3DCOLOR_XRGB(255, 255, 255));
			D3DMATERIAL9 mtrl;
			mtrl.Ambient = white;
			mtrl.Diffuse = white;
			mtrl.Emissive = D3DXCOLOR(D3DCOLOR_XRGB(0, 0, 0));
			mtrl.Power = 2.0f;
			mtrl.Specular = white;

			device->SetMaterial(&mtrl);

			//device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
			//device->SetRenderState(D3DRS_SPECULARENABLE, true);
			//device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME); //填充方式，设置为网格
			//device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			device->SetRenderState(D3DRS_LIGHTING, false);
			auto hr = device->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST,
				0,
				0,
				numVertices,
				0,
				numTriangles);
			device->SetRenderState(D3DRS_LIGHTING, true);
			
		}
	}

	void loadTexture(std::string fileName)
	{
		auto hr = D3DXCreateTextureFromFile(
			device,
			fileName.c_str(),
			&tex
			);
		if (!SUCCEEDED(hr))
		{
			MessageBox(0, "create texture from file failed.", 0, 0);
			return;
		}
	}

	void genTexture()
	{
		auto texWidth = numCellsPerRow;
		auto texHeight = numCellsPerCol;
		auto hr = D3DXCreateTexture(
			device,
			texWidth,
			texHeight,
			0,
			0,
			D3DFMT_X8R8G8B8,
			D3DPOOL_MANAGED,
			&tex);
		if (FAILED(hr))
		{
			MessageBox(0, "create texture failed.", 0, 0);
			return;
		}
		D3DSURFACE_DESC texDesc;
		tex->GetLevelDesc(0, &texDesc);
		if (D3DFMT_X8R8G8B8 != texDesc.Format)
		{
			MessageBox(0, "not suport X8R8G8BR", 0, 0);
			return;
		}
		D3DLOCKED_RECT lockedRect;
		tex->LockRect(0, &lockedRect, 0, 0);
		DWORD *imageData = static_cast<DWORD*>(lockedRect.pBits);
		for (int i = 0; i < texWidth; ++i)
		{
			for (int j = 0; j < texHeight; ++j)
			{
				D3DXCOLOR c;
				auto height = static_cast<float>(getHeightMapEntry(i, j))/heightScale;
				if ((height) < 42.5f) 		 c = D3DUTILITY::BEACH_SAND;
				else if ((height) < 85.0f)	 c = D3DUTILITY::LIGHT_YELLOW_GREEN;
				else if ((height) < 127.5f)  c = D3DUTILITY::PUREGREEN;
				else if ((height) < 170.0f)  c = D3DUTILITY::DARK_YELLOW_GREEN;
				else if ((height) < 212.5f)  c = D3DUTILITY::DARKBROWN;
				else	                     c = D3DUTILITY::WHITE;

				imageData[i*lockedRect.Pitch / 4 + j] = static_cast<D3DCOLOR>(c);
			}
		}
		tex->UnlockRect(0);
		D3DXVECTOR3 lightDirection(0.0f, 1.0f, 0.0f);
		lightTerrain(&lightDirection);
		D3DXFilterTexture(
			tex,
			0,
			0,
			D3DX_DEFAULT);
	}
private:
	void readHeightMapRawFile(const std::string &fileName)
	{
		std::vector<BYTE> in(numVertices);
		std::ifstream inFile(fileName.c_str(), std::ios_base::binary);
		inFile.read((char *)(&in[0]), in.size());
		inFile.close();
		heightmap.reserve(numVertices);
		int index = 0;
		for (auto i : in)
		{
			heightmap.emplace_back(static_cast<int>(i*heightScale));
		}
	}
	void caculateVertices()
	{
		//create vertex buffer
		auto hr = device->CreateVertexBuffer(
			numVertices * sizeof(TerrainVertex),
			D3DUSAGE_WRITEONLY,
			TerrainVertex::FVF,
			D3DPOOL_MANAGED,
			&vb,
			0);
		if (!SUCCEEDED(hr))
		{
			MessageBox(0, "create vertex buffer failed.", 0, 0);
			return;
		}
		auto startX = -width / 2;
		auto startZ = depth / 2;
		auto endX = width / 2;
		auto endZ = -depth / 2;

		auto uCoordIncrementSize = 1.0f / static_cast<float>(numCellsPerRow);
		auto vCoordIncrementSize = 1.0f / static_cast<float>(numCellsPerCol);

		TerrainVertex *v = 0;
		vb->Lock(0, 0, (void **)&v, 0);

		int i = 0;
		for (int z = startZ; z >= endZ; z -= cellSpacing)
		{
			int j = 0;
			for (int x = startX; x <= endX; x += cellSpacing)
			{
				int index = i * numVertsPerRow + j;
				v[index] = TerrainVertex(
					static_cast<float>(x),
					static_cast<float>(heightmap[index]),
					static_cast<float>(z),
					static_cast<float>(j*uCoordIncrementSize),
					static_cast<float>(i*vCoordIncrementSize));
				j++;
			}
			i++;
		}
		vb->Unlock();
	}

	void caculateIndices()
	{
		auto hr = device->CreateIndexBuffer(
			numTriangles * 3 * sizeof(WORD),
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&ib,
			0);
		if (!SUCCEEDED(hr))
		{
			MessageBox(0, "create index buffer failed.", 0, 0);
			return;
		}
		WORD *indices = 0;
		ib->Lock(0, 0, (void **)&indices, 0);

		int baseIndex = 0;
		for (int i = 0; i < numCellsPerCol; ++i)
		{
			for (int j = 0; j < numCellsPerRow; ++j)
			{
				indices[baseIndex] = i * numVertsPerRow + j;
				indices[baseIndex + 1] = i * numVertsPerRow + j + 1;
				indices[baseIndex + 2] = (i + 1) * numVertsPerRow + j;

				indices[baseIndex + 3] = (i + 1) * numVertsPerRow + j;
				indices[baseIndex + 4] = i * numVertsPerRow + j + 1;
				indices[baseIndex + 5] = (i + 1)*numVertsPerRow + j + 1;
				baseIndex += 6;
			}
		}
		ib->Unlock();
	}

	int getHeightMapEntry(int row, int col)
	{
		return heightmap[row*numVertsPerRow + col];
	}

	/*
	*caculate the shade of the block
	*directionToLight: ther revert directiron of the light
	*return: the shading scalar, the range is [0.0, 1.0]
	*/
	float caculateShade(int cellRow, int cellCol, D3DXVECTOR3 *directionToLight)
	{
		auto heightA = static_cast<float>(getHeightMapEntry(cellRow, cellCol));
		auto heightB = static_cast<float>(getHeightMapEntry(cellRow, cellCol + 1));
		auto heightC = static_cast<float>(getHeightMapEntry(cellRow + 1, cellCol));
		D3DXVECTOR3 u(static_cast<float>(cellSpacing), heightB - heightA, 0.0f);
		D3DXVECTOR3 v(0.0f, heightC - heightA, -static_cast<float>(cellSpacing));
		D3DXVECTOR3 n;
		D3DXVec3Cross(&n, &u, &v);
		D3DXVec3Normalize(&n, &n);
		auto cosine = D3DXVec3Dot(&n, directionToLight);
		if (cosine < 0.0f)
			cosine = 0.0f;
		return cosine;
	}

	void lightTerrain(D3DXVECTOR3 *directToLight)
	{
		D3DSURFACE_DESC texDesc;
		auto hr = tex->GetLevelDesc(0, &texDesc);
		if (D3DFMT_X8R8G8B8 != texDesc.Format)
		{
			MessageBox(0, "not support the format.", 0, 0);
			return;
		}
		D3DLOCKED_RECT lockedRec;
		tex->LockRect(
			0,
			&lockedRec,
			0,
			0);
		auto *imageData = static_cast<DWORD*>(lockedRec.pBits);
		for (size_t i = 0; i < texDesc.Height; ++i)
		{
			for (size_t j = 0; j < texDesc.Width; ++j)
			{
				size_t index = i*lockedRec.Pitch / 4 + j;
				D3DXCOLOR c(imageData[index]);
				c *= caculateShade(i, j, directToLight);
				imageData[index] = static_cast<D3DCOLOR>(c);
			}
		}
		tex->UnlockRect(0);
	}
private:
	IDirect3DDevice9 *device;
	IDirect3DTexture9 *tex;
	IDirect3DVertexBuffer9 *vb;
	IDirect3DIndexBuffer9 *ib;

	int numVertsPerRow;
	int numVertsPerCol;

	//the distance between the neighbor point
	int cellSpacing;

	int numCellsPerRow;
	int numCellsPerCol;

	// the width and depth of the height map
	int width;
	int depth;
	int numVertices;
	int numTriangles;

	float heightScale;

	std::vector<int> heightmap;
	struct TerrainVertex
	{
		TerrainVertex() = default;
		TerrainVertex(float x, float y, float z, float u, float v) :
			_x(x), _y(y), _z(z), _u(u), _v(v){}
		float _x, _y, _z;
		float _u, _v;
		static const DWORD FVF;

	};
};

#endif
