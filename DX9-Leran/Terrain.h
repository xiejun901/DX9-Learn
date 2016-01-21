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

		std::vector<BYTE> in(numVertices);
		std::ifstream inFile(heightmapFileName.c_str(), std::ios_base::binary);

		inFile.read((char *)(&in[0]), in.size());

		inFile.close();

		heightmap.resize(numVertices);

		for (size_t i = 0; i < in.size(); ++i)
		{
			heightmap[i] = in[i];
		}

		for (size_t i = 0; i < heightmap.size(); ++i)
		{
			heightmap[i] = static_cast<int>(heightmap[i] *heightScale);
		}

		//create vertex buffer
		auto hr = device->CreateVertexBuffer(
			numVertices * sizeof(TerrainVertex),
			D3DUSAGE_WRITEONLY,
			TerrainVertex::FVF,
			D3DPOOL_MANAGED,
			&vb,
			0);

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
		hr = device->CreateIndexBuffer(
			numTriangles * 3 * sizeof(WORD),
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&ib,
			0);

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

	~Terrain();

	void draw(D3DXMATRIX* world)
	{
		if (device)
		{
			device->SetTransform(D3DTS_WORLD, world);

			device->SetStreamSource(0, vb, 0, sizeof(TerrainVertex));
			device->SetFVF(TerrainVertex::FVF);
			device->SetIndices(ib);


			// turn off lighting since we're lighting it ourselves
			device->SetRenderState(D3DRS_LIGHTING, true);

			auto hr = device->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST,
				0,
				0,
				numVertices,
				0,
				numTriangles);
			device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME); //填充方式，设置为网格
			device->SetRenderState(D3DRS_LIGHTING, true);
			device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		}
	}

private:
	IDirect3DDevice9 *device;
	IDirect3DTexture9 *tex;
	IDirect3DVertexBuffer9 *vb;
	IDirect3DIndexBuffer9 *ib;
	int numVertsPerRow;
	int numVertsPerCol;
	int cellSpacing;

	int numCellsPerRow;
	int numCellsPerCol;
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
			_x(x), _y(y), _z(z) {}//, _u(u), _v(v){}
		float _x, _y, _z;
		//float _u, _v;
		static const DWORD FVF;
	};
};

#endif
