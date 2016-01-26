#ifndef _OBJECT_BASE_H_
#define _OBJECT_BASE_H_
#include <d3d9.h>
#include <d3dx9.h>
class ObjectBase
{
public:
	ObjectBase(IDirect3DDevice9 *d):pDevice(d){}
	virtual void init() = 0;
	virtual void draw(D3DXMATRIX *matWorld = nullptr, D3DXVECTOR3 *lightDirection = nullptr) = 0;
	virtual ~ObjectBase();
protected:
	IDirect3DDevice9 *pDevice;
};


#endif
