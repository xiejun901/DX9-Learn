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
		canArriveWidth = width - 5 * cellSpacing;
		canArriveDepth = depth - 5 * cellSpacing;

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

	void genTexture(D3DXVECTOR3 *lightDirection)
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
				if ((height) < 42.5f) 		 c = d3dUtil::BEACH_SAND;
				else if ((height) < 85.0f)	 c = d3dUtil::LIGHT_YELLOW_GREEN;
				else if ((height) < 127.5f)  c = d3dUtil::PUREGREEN;
				else if ((height) < 170.0f)  c = d3dUtil::DARK_YELLOW_GREEN;
				else if ((height) < 212.5f)  c = d3dUtil::DARKBROWN;
				else	                     c = d3dUtil::WHITE;
				c = d3dUtil::WHITE;
				imageData[i*lockedRect.Pitch / 4 + j] = static_cast<D3DCOLOR>(c);
			}
		}
		tex->UnlockRect(0);
		D3DXVec3Normalize(lightDirection, lightDirection);
		lightTerrain(lightDirection);
		D3DXFilterTexture(
			tex,
			0,
			0,
			D3DX_DEFAULT);
	}

	float getHeight(float x, float z)
	{
		//caculate the position in terrain
		x = static_cast<float>(width) / 2.0f + x;
		z = static_cast<float>(depth) / 2.0f - z;
		x /= static_cast<float>(cellSpacing);
		z /= static_cast<float>(cellSpacing);

		auto col = floorf(x);
		auto row = floorf(z);
		
		/*
		A -- B
        |  / |	
		C -- D
		*/
		auto A = getHeightMapEntry(static_cast<int>(row), static_cast<int>(col));
		auto B = getHeightMapEntry(static_cast<int>(row), static_cast<int>(col) + 1);
		auto C = getHeightMapEntry(static_cast<int>(row) + 1, static_cast<int>(col));
		auto D = getHeightMapEntry(static_cast<int>(row) + 1, static_cast<int>(col) + 1);

		auto dx = x - col;
		auto dz = z - row;

		auto height = 0.0f;
		if (dz < 1.0f - dx)
		{
			auto uy = B - A;
			auto vy = C - A;
			height = A + uy * dx + vy * dz;
		}
		else
		{
			auto uy = C - D;
			auto vy = B - D;
			height = D + uy*(1 - dx) + vy*(1 - dz);
		}
		return height;
	}

	void transfrom(D3DXVECTOR3 *out, D3DXVECTOR3 *in)
	{
		if (in && out)
		{
			auto x = 0.0f;
			auto y = 0.0f;
			auto z = 0.0f;
			if (in->x < -static_cast<float>(canArriveWidth) / 2)
				x = -static_cast<float>(canArriveWidth) / 2.0f;
			else if (in->x > static_cast<float>(canArriveWidth) / 2)
				x = static_cast<float>(canArriveWidth) / 2;
			else
				x = in->x;
			if (in->z < -static_cast<float>(canArriveDepth) / 2)
				z = -static_cast<float>(canArriveDepth) / 2;
			else if (in->z > static_cast<float>(canArriveDepth) / 2)
				z = static_cast<float>(canArriveDepth) / 2;
			else
				z = in->z;
			auto height = getHeight(x, z);
			if (in->y < height + 5.0f)
				y = height + 5.0f;
			else
				y = in->y;
			out->x = x;
			out->y = y;
			out->z = z;
		}
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
				c = c / 2.0f + c*caculateShade(i, j, directToLight) / 2.0f;
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
	int canArriveWidth;
	int canArriveDepth;
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
