#include <windowsx.h>
#include <algorithm>  
#include "Light.h"

Light::Light()
{
	_directional = true;
	_Lpos.SetX(0.0f);
	_Lpos.SetY(0.0f);
	_Lpos.SetZ(1.0f);
	_Lpos.SetW(1.0f);
	_La = RGB(32, 32, 32);
	_Ld = RGB(128, 128, 128);
	_Ls = RGB(255, 255, 255);
}

Light::Light(const Light& other)
{
	_directional = other.IsDirectional();
	_Lpos = other.GetLPos();
	_La = other.GetColorLa();
	_Ld = other.GetColorLd();
	_Ls = other.GetColorLs();
}

Light::~Light()
{
}

bool Light::IsDirectional() const
{
	return _directional;
}

void Light::SetDirectional(bool value)
{
	_directional = value;
}

Vertex Light::GetLPos() const
{
	return _Lpos;
}

void Light::SetLPos(Vertex & pos)
{
	_Lpos = pos;
}

COLORREF Light::GetColorLa() const
{
	return _La;
}

void Light::SetColorLa(COLORREF color)
{
	_La = color;
}

COLORREF Light::GetColorLd() const
{
	return _Ld;
}

void Light::SetColorLd(COLORREF color)
{
	_Ld = color;
}

COLORREF Light::GetColorLs() const
{
	return _Ls;
}

void Light::SetColorLs(COLORREF color)
{
	_Ls = color;
}



