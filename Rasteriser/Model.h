#pragma once

#include <windows.h>
#include <vector>
#include "Light.h"
#include "Vertex_4D.h"
#include "Matrix_4D.h"

using namespace std;

class Triangle
{
public:
	Triangle();
	Triangle(int index0, int index1, int index2);
	~Triangle();
	Triangle(const Triangle& other);
	// Accessor to return index of a specified vertex
	int GetIndex(int i) const;
	// Accessors to triangle's normal
	Vertex GetNormal() const;
	void SetNormal(Vertex & normal);
	// Accessors to triangle's centroid
	Vertex GetCenter() const;
	void SetCenter(Vertex& center);
	// Accessors to triangle's backface status
	bool IsVisible() const;
	void SetVisible(bool value);
	// Accessors to triangle's color
	COLORREF GetColor() const;
	void SetColor(COLORREF color);
	COLORREF GetVertexColor(int i) const;
	void SetVertexColor(int i, COLORREF color);
		
	// Assignment operator
	Triangle& operator= (const Triangle &rhs);

private:
	int _indices[3];
	bool _visible;
	Vertex _normal;
	Vertex _center;
	COLORREF _color;
	COLORREF _vcolor[3];
};

class Model
{
public:
	Model();
	~Model();
	
	//Clear Model Data
	void Clear();
	void ClearVertices();
	void ClearNormals();
	void ClearTriangles();

	// Accessors
	vector<Triangle>& GetTriangles();
	vector<Vertex>& GetVertices();
	size_t GetTriangleCount() const;
	size_t GetVertexCount() const;
	void AddVertex(float x, float y, float z);
	void AddTriangle(int i0, int i1, int i2);

	void CopyModel(Model& rhs);

	// Apply a transformation matrix onto model
	void ApplyTransform(const Matrix &transform);
	// Dehomogenise model's co-ordinates
	void Dehomogenise();

	// Calculates the Normals of a Model
	void CalculateVertexNormals();
	// Calculates the Normals of a Model
	void CalculateNormals(bool CCW);
	// Calculates Face Center of Model
	void CalculateCenters();
	// Marks Backfaces of Model
	void MarkBackfaces(float eX, float eY, float eZ, bool ortho);
	// Apply Depth Sorting on Model Triangles
	void ApplyDepthSorting();
	// Calculate per triangle flat-shading
	void CalcFlatShading(HDC hdc, Light light, float eX, float eY, float eZ, bool ortho);
	// Calculate per vertex shading
	void CalcVertexShading(HDC hdc, Light light, float eX, float eY, float eZ, bool ortho);

	// Accessors to model's colors
	COLORREF GetColorKa() const;
	void SetColorKa(COLORREF color);
	COLORREF GetColorKd() const;
	void SetColorKd(COLORREF color);
	COLORREF GetColorKs() const;
	void SetColorKs(COLORREF color);
	float GetShininess() const;
	void SetShininess(float value);

	// Draw model in a wire-frame fashion
	void DrawWireFrame(HDC hdc);
	// Draw model in a flood-fill fashion
	void DrawSolidFill(HDC hdc);
	// Draw model in a flat-shading fashion
	void DrawFlatShading(HDC hdc);
	// Draw model in a vertex-shading fashion
	void DrawVertexShading(HDC hdc);

private:
	vector<Triangle> _triangles;
	vector<Vertex> _vertices;
	vector<Vertex> _normals;
	COLORREF _Ka;
	COLORREF _Kd;
	COLORREF _Ks;
	int _shininess;

};

