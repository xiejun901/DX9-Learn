#ifndef _SNOW_BOY_H_
#define _SNOW_BOY_H_

#include<string>
#include "d3dUtility.h"
#include "ObjectBase.h"

class SnowBoy:public ObjectBase
{
public:
	SnowBoy(IDirect3DDevice9 *d, 
		const std::string &headTex = "snowhead.jpg", 
		const std::string &bodyTex = "snowbody.jpg") :
		ObjectBase(d),
		headTextureFileName(headTex),
		bodyTextureFileName(bodyTex){}
	void init() override;
	void draw(D3DXMATRIX *matWorld = nullptr, D3DXVECTOR3 *lightDirection = nullptr) override;
	~SnowBoy();

private:
	void loadTexture();
	ID3DXMesh *createTextureSphere(float radius, UINT slices, UINT stacks);
	void light(ID3DXMesh *mesh, D3DXVECTOR3 directionToLight);
	void lightSnowBoy(const D3DXVECTOR3 &directionToLigh);
private:
    ID3DXMesh *body;
    ID3DXMesh *head;
    IDirect3DTexture9 *headTexture;
    IDirect3DTexture9 *bodyTexture;
    static D3DXMATRIX headMatTrans;
	std::string headTextureFileName;
	std::string bodyTextureFileName;

    struct Vertex
    {
        float x, y, z;
        float nx, ny, nz;
        DWORD diffuse;
        float u, v;
        enum FVF
        {
            FVF_Flags = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1
        };
    };

};

#endif