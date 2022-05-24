#include <math.h>
#include "Matrix_4D.h"

#define PI 3.14159265358

Matrix::Matrix()
{
	SetIdentity();
}

Matrix::Matrix(const Matrix& other)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			SetM(i,j, other.GetM(i,j));
}

Matrix::~Matrix()
{

}

void Matrix::SetZero()
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			_m[i][j] = 0.0f;
}

void Matrix::SetIdentity()
{
	SetZero();
	_m[0][0] = _m[1][1] = _m[2][2] = _m[3][3] = 1.0f;
}

void Matrix::SetMatrix(const Matrix& other)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			_m[i][j] = other.GetM(i,j);
}

float Matrix::GetM(const int row, const int col) const
{
	return _m[row][col];
}

void Matrix::SetM(const int row, const int col, const float val)
{
	_m[row][col] = val;
}


const Matrix  Matrix::operator* (const Matrix& other) const
{
	Matrix result;

	result.SetM(0,0, GetM(0,0)*other.GetM(0,0)+GetM(0,1)*other.GetM(1,0)+GetM(0,2)*other.GetM(2,0)+GetM(0,3)*other.GetM(3,0));
	result.SetM(1,0, GetM(1,0)*other.GetM(0,0)+GetM(1,1)*other.GetM(1,0)+GetM(1,2)*other.GetM(2,0)+GetM(1,3)*other.GetM(3,0));
	result.SetM(2,0, GetM(2,0)*other.GetM(0,0)+GetM(2,1)*other.GetM(1,0)+GetM(2,2)*other.GetM(2,0)+GetM(2,3)*other.GetM(3,0));
	result.SetM(3,0, GetM(3,0)*other.GetM(0,0)+GetM(3,1)*other.GetM(1,0)+GetM(3,2)*other.GetM(2,0)+GetM(3,3)*other.GetM(3,0));

	result.SetM(0,1, GetM(0,0)*other.GetM(0,1)+GetM(0,1)*other.GetM(1,1)+GetM(0,2)*other.GetM(2,1)+GetM(0,3)*other.GetM(3,1));
	result.SetM(1,1, GetM(1,0)*other.GetM(0,1)+GetM(1,1)*other.GetM(1,1)+GetM(1,2)*other.GetM(2,1)+GetM(1,3)*other.GetM(3,1));
	result.SetM(2,1, GetM(2,0)*other.GetM(0,1)+GetM(2,1)*other.GetM(1,1)+GetM(2,2)*other.GetM(2,1)+GetM(2,3)*other.GetM(3,1));
	result.SetM(3,1, GetM(3,0)*other.GetM(0,1)+GetM(3,1)*other.GetM(1,1)+GetM(3,2)*other.GetM(2,1)+GetM(3,3)*other.GetM(3,1));

	result.SetM(0,2, GetM(0,0)*other.GetM(0,2)+GetM(0,1)*other.GetM(1,2)+GetM(0,2)*other.GetM(2,2)+GetM(0,3)*other.GetM(3,2));
	result.SetM(1,2, GetM(1,0)*other.GetM(0,2)+GetM(1,1)*other.GetM(1,2)+GetM(1,2)*other.GetM(2,2)+GetM(1,3)*other.GetM(3,2));
	result.SetM(2,2, GetM(2,0)*other.GetM(0,2)+GetM(2,1)*other.GetM(1,2)+GetM(2,2)*other.GetM(2,2)+GetM(2,3)*other.GetM(3,2));
	result.SetM(3,2, GetM(3,0)*other.GetM(0,2)+GetM(3,1)*other.GetM(1,2)+GetM(3,2)*other.GetM(2,2)+GetM(3,3)*other.GetM(3,2));

	result.SetM(0,3, GetM(0,0)*other.GetM(0,3)+GetM(0,1)*other.GetM(1,3)+GetM(0,2)*other.GetM(2,3)+GetM(0,3)*other.GetM(3,3));
	result.SetM(1,3, GetM(1,0)*other.GetM(0,3)+GetM(1,1)*other.GetM(1,3)+GetM(1,2)*other.GetM(2,3)+GetM(1,3)*other.GetM(3,3));
	result.SetM(2,3, GetM(2,0)*other.GetM(0,3)+GetM(2,1)*other.GetM(1,3)+GetM(2,2)*other.GetM(2,3)+GetM(2,3)*other.GetM(3,3));
	result.SetM(3,3, GetM(3,0)*other.GetM(0,3)+GetM(3,1)*other.GetM(1,3)+GetM(3,2)*other.GetM(2,3)+GetM(3,3)*other.GetM(3,3));
	
	return result;
}

Matrix& Matrix::operator= (const Matrix &rhs)
{
	if (this != &rhs)
	{
		SetMatrix(rhs);
	}
	return *this;

}

const Vertex Matrix::operator*(const Vertex &other) const
{
	Vertex result;
	
	result.SetX( GetM(0,0)*other.GetX()+GetM(0,1)*other.GetY()+GetM(0,2)*other.GetZ()+GetM(0,3)*other.GetW());
	result.SetY( GetM(1,0)*other.GetX()+GetM(1,1)*other.GetY()+GetM(1,2)*other.GetZ()+GetM(1,3)*other.GetW());
	result.SetZ( GetM(2,0)*other.GetX()+GetM(2,1)*other.GetY()+GetM(2,2)*other.GetZ()+GetM(2,3)*other.GetW());
	result.SetW( GetM(3,0)*other.GetX()+GetM(3,1)*other.GetY()+GetM(3,2)*other.GetZ()+GetM(3,3)*other.GetW());
	
	return result;
}

void Matrix::SetRotationX(float degrees)
{
	float radians = degrees*PI / 180.0f;
	SetIdentity();
	_m[1][1] =  cos(radians); _m[1][2] = -sin(radians); 
	_m[2][1] =  sin(radians); _m[2][2] =  cos(radians);
}

void Matrix::SetRotationY(float degrees)
{
	float radians = degrees*PI / 180.0f;
	SetIdentity();
	_m[0][0] =  cos(radians); _m[0][2] =  sin(radians); 
	_m[2][0] = -sin(radians); _m[2][2] =  cos(radians);
}

void Matrix::SetRotationZ(float degrees)
{
	float radians = degrees*PI / 180.0f;
	SetIdentity();
	_m[0][0] =  cos(radians); _m[0][1] = -sin(radians); 
	_m[1][0] =  sin(radians); _m[1][1] =  cos(radians);
}

void Matrix::SetTranslate(float tx, float ty, float tz)
{
	SetIdentity();
	_m[0][3] = tx;
	_m[1][3] = ty; 
	_m[2][3] = tz;
	
}

void Matrix::SetScale(float sx, float sy, float sz)
{
	SetIdentity();
	_m[0][0] = sx;
	_m[1][1] = sy; 
	_m[2][2] = sz;
}

void Matrix::SetViewMatrix(const Vertex& camera_position, const Vertex& view_vector, const Vertex& up_vector)
{
}


void Matrix::SetCamera(float xRotation, float yRotation, float zRotation, float xPosition, float yPosition, float zPosition)
{
	Matrix Camera, CameraPos, CameraRotX, CameraRotY, CameraRotZ;
	CameraPos.SetTranslate(-xPosition, -yPosition, -zPosition);
	CameraRotX.SetRotationX(-xRotation);
	CameraRotY.SetRotationY(-yRotation);
	CameraRotZ.SetRotationZ(-zRotation);
	Camera = CameraPos*CameraRotZ*CameraRotY*CameraRotX;
	SetMatrix(Camera);
}

void Matrix::SetOrtho(float d)
{
	SetZero();
	_m[0][0] = d;// 1.0f;
	_m[1][1] = d;// 1.0f;
	_m[2][2] = d;// 1.0f;
	_m[3][3] = 1.0f;

}

void Matrix::SetPerspective(float d, float aspectRatio)
{
	SetZero();
	_m[0][0] = -d/aspectRatio;
	_m[1][1] = -d; 
	_m[2][2] = -d; //1.0; //for keeping depth
	_m[3][2] = 1.0f;

}

void Matrix::SetViewvolume(float left, float right, float bottom, float top, float front, float rear)
{
	float width = right-left;
	float height = top-bottom;
	float depth = front-rear;
	SetIdentity();
	_m[0][0] = 1.0f/width;
	_m[1][1] = 1.0f/height; 
	_m[2][2] = 1.0f/depth; 
}

void Matrix::SetViewport(int left, int right, int top, int bottom)
{
	int width = right-left;
	int height = bottom-top;
	SetIdentity();
	_m[0][0] = width;
	_m[1][1] = -height; 
	_m[2][2] = 1.0f; //for keeping depth
	_m[0][3] = left+width/2.0f;
	_m[1][3] = top+height/2.0f;
	_m[2][3] = 0.0f; //depth buffer
}

