#ifndef _TERRAIN_H_
#define _TERRAIN_H_
#include<d3d9.h>
#include<d3dx9.h>
#include<string>
#include<vector>
#include<fstream>
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
			//device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME); //填充方式，设置为网格
			device->SetRenderState(D3DRS_LIGHTING, false);
			device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			auto hr = device->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST,
				0,
				0,
				numVertices,
				0,
				numTriangles);
			
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
