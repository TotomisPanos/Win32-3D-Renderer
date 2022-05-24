#include "Framework.h"
#include "Winuser.h"
#include <iostream>
#include "time.h"
#include "Rasteriser.h"

#define DEFAULT_FRAMERATE 30
#define DEFAULT_WIDTH	  800
#define DEFAULT_HEIGHT	  800

// Reference to ourselves - primarily used to access the message handler correctly
Framework *	_thisFramework = NULL;

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

// The entry point for a Windows application
int APIENTRY wWinMain(_In_	   HINSTANCE hInstance,
				  	  _In_opt_ HINSTANCE hPrevInstance,
					  _In_	   LPWSTR    lpCmdLine,
					  _In_	   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	if (_thisFramework)
	{
		return _thisFramework->Run(hInstance, nCmdShow);
	}
	return -1;
}


Framework::Framework()
{
	_thisFramework = this;
}

Framework::~Framework()
{
}

int Framework::Run(HINSTANCE hInstance, int nCmdShow)
{
	_hInstance = hInstance;
	if (!InitialiseMainWindow(nCmdShow))
	{
		return -1;
	}
	return MainLoop();
}

// Main program loop.  

int Framework::MainLoop()
{
	MSG msg;
	HACCEL hAccelTable = LoadAccelerators(_hInstance, MAKEINTRESOURCE(IDC_RASTERISER));
	LARGE_INTEGER counterFrequency;
	LARGE_INTEGER nextTime;
	LARGE_INTEGER currentTime;
	LARGE_INTEGER lastTime;
	bool updateFlag = true;

	// Initialise timer
	QueryPerformanceFrequency(&counterFrequency);
	DWORD msPerFrame = (DWORD)(counterFrequency.QuadPart / DEFAULT_FRAMERATE);
	double timeFactor = 1.0 / counterFrequency.QuadPart;
	QueryPerformanceCounter(&nextTime);
	lastTime = nextTime;

	// Main message loop:
	msg.message = WM_NULL;
	while (msg.message != WM_QUIT)
	{
		if (updateFlag)
		{
			QueryPerformanceCounter(&currentTime);
			_timeSpan = (currentTime.QuadPart - lastTime.QuadPart) * timeFactor;
			lastTime = currentTime;
			Update(_bitmap);
			updateFlag = false;
		}
		QueryPerformanceCounter(&currentTime);
		// Is it time to render the frame?
		if (currentTime.QuadPart > nextTime.QuadPart && !PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE))
		{
			RenderScene(_bitmap.GetDC(), _bitmap);
			// Make sure that the window gets repainted
			InvalidateRect(_hWnd, NULL, FALSE);
			// Set time for next frame
			nextTime.QuadPart += msPerFrame;
			// If we get more than a frame ahead, allow one to be dropped
			// Otherwise, we will never catch up if we let the error accumulate
			// and message handling will suffer
			if (nextTime.QuadPart < currentTime.QuadPart)
			{
				nextTime.QuadPart = currentTime.QuadPart + msPerFrame;
			}
			updateFlag = true;
		}
		else
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
	}
	return (int)msg.wParam;
}

bool Framework::InitialiseMainWindow(int nCmdShow)
{
	#define MAX_LOADSTRING 100

	WCHAR windowTitle[MAX_LOADSTRING];          
	WCHAR windowClass[MAX_LOADSTRING];            
	
	LoadStringW(_hInstance, IDS_APP_TITLE, windowTitle, MAX_LOADSTRING);
	LoadStringW(_hInstance, IDC_RASTERISER, windowClass, MAX_LOADSTRING);

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = _hInstance;
	wcex.hIcon = LoadIcon(_hInstance, MAKEINTRESOURCE(IDI_RASTERISER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = windowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	if (!RegisterClassExW(&wcex))
	{
		return false;
	}

	_hWnd = CreateWindowW(windowClass, 
						  windowTitle, 
					      WS_OVERLAPPEDWINDOW,
						  CW_USEDEFAULT, 0, 800, 800, 
					      nullptr, nullptr, _hInstance, nullptr);
	if (!_hWnd)
	{
		return false;
	}

	
	InitializeScene(_hWnd);

	// Create a bitmap of the same size as the client area of the window.
	RECT clientArea;
	GetClientRect(_hWnd, &clientArea);
	_bitmap.Create(_hWnd, clientArea.right - clientArea.left, clientArea.bottom - clientArea.top);

	ShowWindow(_hWnd, nCmdShow);
	UpdateWindow(_hWnd);

	return true;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (_thisFramework != NULL)
	{
		// If framework is started, then we can call our own message proc
		return _thisFramework->MsgProc(hWnd, message, wParam, lParam);
	}
	else
	{
		// otherwise, we just pass control to the default message proc
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

LRESULT Framework::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	switch (message)
	{
		case WM_PAINT:
		{
			BitBlt(hdc, 0, 0, _bitmap.GetWidth(), _bitmap.GetHeight(), _bitmap.GetDC(), 0, 0, SRCCOPY);
			EndPaint(hWnd, &ps);

			RenderScene(hdc, _bitmap);

			EndPaint(hWnd, &ps);
		}
		break;

		case WM_SIZE:
		{
			// Delete any existing bitmap and create a new one of the required size.
			_bitmap.Create(hWnd, LOWORD(lParam), HIWORD(lParam));
			// Now render to the resized bitmap
			Update(_bitmap);
			RenderScene(hdc, _bitmap);
			InvalidateRect(hWnd, NULL, FALSE);
		}

		// On mouse keys pressing
		case WM_LBUTTONDOWN:
		{
			SetRenderParams(0);
			InvalidateRect(hWnd, NULL, TRUE);
			break; 
		}
		case WM_RBUTTONDOWN:
		{
			SetRenderParams(4);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
		case WM_LBUTTONUP:
		{
			//OnLButtonUp(hWnd, lParam);
			break;
		}
		case WM_MOUSEMOVE:
		{
			//OnMouseMove(hWnd, lParam);
			break;
		}
		case WM_ERASEBKGND:
		{
			return TRUE;
		}

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		default:
		{
			return DefWindowProc(hWnd, message, wParam, lParam); 
		}
	}
	return 0;
}

void Framework::InitializeScene(HWND _hWnd)
{

}

void Framework::RenderScene(HDC hdc, Bitmap& bitmap)
{
}

void Framework::SetRenderParams(int action)
{
}

void Framework::Update(Bitmap& bitmap)
{
}

void Framework::DrawString(HDC hdc, LPCTSTR text)
{
}


