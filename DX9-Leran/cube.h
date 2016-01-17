#ifndef _CUBE_H_
#define _CUBE_H_
#include <d3dx9.h>
struct Vertex {
    Vertex() {}
    Vertex(float x, float y, float z) :_x(x),_y(y),_z(z){}
    float _x, _y, _z;
    static const DWORD FVF;
};

#endif
