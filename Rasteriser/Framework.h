#pragma once

#include <windows.h>
#include "resource.h"
#include "Model.h"
#include "Matrix_4D.h"
#include "Bitmap.h"

#include <winresrc.h>
#include "targetver.h"

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

class Framework
{
public:
	Framework();
	virtual ~Framework();

	int Run(HINSTANCE hInstance, int nCmdShow);

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual void InitializeScene(HWND _hWnd);
	virtual void RenderScene(HDC hdc, Bitmap& bitmap);

	virtual void SetRenderParams(int action);
	virtual void Update(Bitmap& bitmap);
	virtual void DrawString(HDC hdc, LPCTSTR text);


private:
	HINSTANCE	_hInstance;
	HWND		_hWnd;
	Bitmap		_bitmap;

	// Used in timing loop
	double			_timeSpan;

	bool InitialiseMainWindow(int nCmdShow);
	int MainLoop();

};