#pragma once

#include "Vertex_4D.h"

class Matrix
{
public:

	// Default constructor
	Matrix();
	// Copy constructor
	Matrix(const Matrix& other);
	// Destructor
	~Matrix();
	// Retrieve value in matrix at specified row and column
	float GetM(const int row, const int col) const;
	// Set value in matrix at specified row and column
	void SetM(const int row, const int col, const float val);
	// Copy assignment operator
	Matrix& operator= (const Matrix &rhs);
	// Multiply two matrices together
	const Matrix operator*(const Matrix &other) const;
	// Multiply a matrix by a vertex, returning a vertex
	const Vertex operator*(const Vertex &other) const;

	// Set values for special matrices
	void				SetZero();
	void				SetIdentity();

	// Copy values of a matrix
	void				SetMatrix(const Matrix& other);

	// Set values for transformation matrices
	void				SetRotationX(float degrees);
	void				SetRotationY(float degrees);
	void				SetRotationZ(float degrees);
	void				SetTranslate(float tx, float ty, float tz);
	void				SetScale(float sx, float sy, float sz);
    // Set values for viewing matrices
	void				SetViewMatrix(const Vertex& camera_position, const Vertex& view_vector, const Vertex& up_vector);
	void				SetCamera(float xRotation, float yRotation, float zRotation, float xPosition, float yPosition, float zPosition);
	void				SetOrtho(float d);
	void				SetPerspective(float d, float aspectRatio);
	void                SetViewvolume(float left, float right, float bottom, float top, float front, float rear);
	void				SetViewport(int left, int right, int top, int bottom);
	

private:
	float				_m[4][4];
};
