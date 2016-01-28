#ifndef _CAMERA_H_
#define _CAMERA_H_
#include <d3d9.h>
#include <d3dx9.h>
class Camera
{
public:
    enum CameraType
    {
        LANDOBJECT,
        AIRCRAFT
    };
	Camera();
	void strafe(float units);
	void fly(float units);
	void walk(float units);

	void pitch(float angle);
	void yaw(float angle);
	void roll(float angle);

	void getViewMatrix(D3DXMATRIX *v);
	void getPosition(D3DXVECTOR3 *pos);
	void setPosition(D3DXVECTOR3 *pos);
	void getRight(D3DXVECTOR3 *right);
	void getUp(D3DXVECTOR3 *up);
	void getLook(D3DXVECTOR3 *look);

	~Camera();

private:
	D3DXVECTOR3 _right;
	D3DXVECTOR3 _up;
	D3DXVECTOR3 _look;
	D3DXVECTOR3 _pos;
    CameraType cameraType;
};


#endif
