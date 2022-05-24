#pragma once

class Vertex
{
public:
	Vertex();
	Vertex(float x, float y, float z);

	// Copy constructor.  
	Vertex(const Vertex& v);

	// Destructor
	~Vertex();

	void Dehomogenise();
	void Normalise();
	float Length();

	// Accessors
	float GetX() const;
	void SetX(const float x);
	float GetY() const;
	void SetY(const float y);
	float GetZ() const;
	void SetZ(const float z);
	float GetW() const;
	void SetW(const float w);

	// Assignment operator
	Vertex& operator= (const Vertex& rhs);
	bool operator== (const Vertex& rhs) const;

	// Vector addition and substraction
	const Vertex operator+ (const Vertex& rhs) const;
	const Vertex operator- (const Vertex& rhs) const;

	// Vector dot and cross products
	float DotProduct(const Vertex& rhs) const;
	Vertex CrossProduct(const Vertex& rhs) const;

private:
	float _x;
	float _y;
	float _z;
	float _w;
};

