#include "Camera.h"

Camera::Camera():_look(D3DXVECTOR3(0.0f,0.0f,1.0f)),
_right(D3DXVECTOR3(1.0f, 0.0f,0.0f)),
_up(D3DXVECTOR3(0.0f,1.0f,0.0f)),
_pos(D3DXVECTOR3(0.0f,0.0f,0.0f))
{
}

void Camera::strafe(float units)
{
	_pos += _right * units;
}

void Camera::fly(float units)
{
	_pos += _up*units;
}

void Camera::walk(float units)
{
	_pos += _look*units;
}

void Camera::pitch(float angle)
{
	D3DXMATRIX t;
	D3DXMatrixRotationAxis(&t, &_right, angle);
	D3DXVec3TransformCoord(&_up, &_up, &t);
	D3DXVec3TransformCoord(&_look, &_look, &t);
}

void Camera::yaw(float angle)
{
	D3DXMATRIX t;
	D3DXMatrixRotationAxis(&t, &_up, angle);
	D3DXVec3TransformCoord(&_right, &_right, &t);
	D3DXVec3TransformCoord(&_look, &_look, &t);
}

void Camera::roll(float angle)
{
	D3DXMATRIX t;
	D3DXMatrixRotationAxis(&t, &_look, angle);
	D3DXVec3TransformCoord(&_up, &_up, &t);
	D3DXVec3TransformCoord(&_right, &_right, &t);
}


Camera::~Camera()
{
}

void Camera::getViewMatrix(D3DXMATRIX * v)
{
	//重新计算使得right, up, look 三个向量正交以及为单位向量
	D3DXVec3Normalize(&_look, &_look);

	D3DXVec3Cross(&_up, &_look, &_right);
	D3DXVec3Normalize(&_up, &_up);

	D3DXVec3Cross(&_right, &_up, &_look);
	D3DXVec3Normalize(&_right, &_right);
	
	//构造观察矩阵
	auto x = -D3DXVec3Dot(&_right, &_pos);
	auto y = -D3DXVec3Dot(&_up, &_pos);
	auto z = -D3DXVec3Dot(&_look, &_pos);

	(*v)(0, 0) = _right.x;
	(*v)(0, 1) = _up.x;
	(*v)(0, 2) = _look.x;
	(*v)(0, 3) = 0.0f;

	(*v)(1, 0) = _right.y;
	(*v)(1, 1) = _up.y;
	(*v)(1, 2) = _look.y;
	(*v)(1, 3) = 0.0f;

	(*v)(2, 0) = _right.z;
	(*v)(2, 1) = _up.z;
	(*v)(2, 2) = _look.z;
	(*v)(2, 3) = 0.0f;

	(*v)(3, 0) = x;
	(*v)(3, 1) = y;
	(*v)(3, 2) = z;
	(*v)(3, 3) = 1.0f;
}

void Camera::getPosition(D3DXVECTOR3 * pos)
{
	*pos = _pos;
}

void Camera::setPoistion(D3DXVECTOR3 * pos)
{
	_pos = *pos;
}

void Camera::getRight(D3DXVECTOR3 * right)
{
	*right = _right;
}

void Camera::getUp(D3DXVECTOR3 * up)
{
	*up = _up;
}

void Camera::getLook(D3DXVECTOR3 * look)
{
	*look = _look;
}
