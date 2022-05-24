#include <windowsx.h>
#include "MD2Loader.h"
#include "Rasteriser.h"

Rasteriser app;

void Rasteriser::AddModel(Model & model)
{
	_models.push_back(model);
}

void Rasteriser::TransformModel(int model_inx, Matrix transform)
{
	if (model_inx > _models.size() - 1)
	{
		return;
	}
	_models[model_inx].ApplyTransform(transform);
}

void Rasteriser::SetViewTransform(Matrix view)
{
	_view.SetMatrix(view);
}

void Rasteriser::SetCameraTransform(Matrix cam)
{
	_camera.SetMatrix(cam);
}

void Rasteriser::SetProjectionTransform(Matrix proj)
{
	_projection.SetMatrix(proj);
}

void Rasteriser::SetViewport(int left, int right, int top, int bottom)
{
	_viewportrect.left = left;
	_viewportrect.top = top;
	_viewportrect.right = right;
	_viewportrect.bottom = bottom;
	_viewport.SetViewport(left, right, top, bottom);
}

void Rasteriser::DrawViewportFrame(HDC hdc)
{
	POINT points[4];
	HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 0, 255));
	HGDIOBJ oldPen = SelectPen(hdc, pen);

	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
	HBRUSH oldBrush = SelectBrush(hdc, brush);

	points[0].x = (int)_viewportrect.left;      points[0].y = (int)_viewportrect.top;
	points[1].x = (int)_viewportrect.right - 1; points[1].y = (int)_viewportrect.top;
	points[2].x = (int)_viewportrect.right - 1; points[2].y = (int)_viewportrect.bottom - 1;
	points[3].x = (int)_viewportrect.left;      points[3].y = (int)_viewportrect.bottom - 1;

	Polygon(hdc, points, 4);

	SelectBrush(hdc, oldBrush);
	DeleteObject(brush);

	SelectPen(hdc, oldPen);
	DeleteObject(pen);
	
	pen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	SelectObject(hdc, pen);

	int width = (int)_viewportrect.right - (int)_viewportrect.left;
	int height = (int)_viewportrect.bottom - (int)_viewportrect.top;

	MoveToEx(hdc, (int)_viewportrect.left, (int)_viewportrect.top + height / 2, NULL);
	LineTo(hdc, (int)_viewportrect.right - 1, (int)_viewportrect.top + height / 2);

	MoveToEx(hdc, (int)_viewportrect.left + width / 2, (int)_viewportrect.top, NULL);
	LineTo(hdc, (int)_viewportrect.left + width / 2, (int)_viewportrect.bottom - 1);
	
		
	SelectObject(hdc, oldPen);
	DeleteObject(pen);
}

void Rasteriser::DrawString(Bitmap &bitmap, LPCTSTR text)
{
	HDC hdc = bitmap.GetDC();

	HFONT hFont = CreateFont(54, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Myfont"));
 
	HFONT holdFont = (HFONT)SelectObject(hdc, hFont);

	if (holdFont)
	{
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkColor(hdc, RGB(20, 20, 20));

		// Display the text string. 
		TextOut(hdc, 0, 0, text, lstrlen(text));

		// Restore the original font. 
		SelectObject(hdc, holdFont);
	}
	DeleteObject(hFont);
}

void Rasteriser::DrawString(HDC hdc, int xPos, int yPos, int fSize, COLORREF textColor, LPCTSTR text)
{
	HFONT hFont, hOldFont;

	// Retrieve a handle to the variable stock font.  
	hFont = hFont = CreateFont(fSize, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Myfont"));

	// Select the variable stock font into the specified device context. 
	if (hOldFont = (HFONT)SelectObject(hdc, hFont))
	{
		//SetTextColor(hdc, RGB(255, 255, 255));
		SetTextColor(hdc, textColor);
		SetBkColor(hdc, RGB(255, 255, 255));

		// Display the text string.  
		TextOut(hdc, xPos, yPos, text, lstrlen(text));

		// Restore the original font.        
		SelectObject(hdc, hOldFont);
	}
	DeleteObject(hFont);
}

void Rasteriser::RenderScene(HDC hdc, Bitmap& bitmap)
{
	Model model;
	Vertex lightPos;
	
	// Clear the window to black
	bitmap.Clear(RGB(0, 0, 0));

	float eX, eY, eZ; // Get the eye position
	eX = -_camera.GetM(0,3);
	eY = -_camera.GetM(1,3);
	eZ = -_camera.GetM(2,3);

	for (size_t i = 0; i < _models.size(); i++)
	{
		model = _models[i];

		model.ApplyTransform(_camera);

		model.CalculateCenters();

		if (_depthsort)
		{
			model.ApplyDepthSorting();
		}

		model.CalculateCenters();
		model.CalculateNormals(true);


		lightPos = _camera * _lPos;
		_light.SetLPos(lightPos);
		
		if (_render_mode == 2)
		{
			model.CalcFlatShading(hdc, _light, eX, eY, eZ, _ortho);
		}

		if (_render_mode == 3)
		{
			model.CalculateVertexNormals(); 
			model.CalcVertexShading(hdc, _light, eX, eY, eZ, _ortho);
			model.DrawVertexShading(hdc);
		}
		
		model.ApplyTransform(_projection);
		model.Dehomogenise();

		model.CalculateCenters();
		model.CalculateNormals(true);

	
		if (!_facecull)
		{
			model.MarkBackfaces(eX, eY, eZ, _ortho);
		}

		model.ApplyTransform(_viewport);

		switch (_render_mode)
		{
			case  0: 
			{
				model.DrawWireFrame(hdc);
				break;
			}
			case  1: 
			{
				model.DrawSolidFill(hdc);
				break;
			}
			case  2: 
			{
				model.DrawFlatShading(hdc);
				break;
			}
			case  3: 
			{
				model.DrawVertexShading(hdc);
				break;
			}
			default: 
			{
				model.DrawWireFrame(hdc);
				break;
			}
		}
	}	
}

void Rasteriser::InitializeScene(HWND _hWnd)
{
	// Load a model and add it in scene's model list
	Model MyModel;
	MD2Loader MyLoader;

	//Load and Add the first model
	MyLoader.LoadModel("Models\\marvin.md2", MyModel);
	MyModel.SetColorKa(RGB(45, 3, 3));
	MyModel.SetColorKd(RGB(196, 54, 54));
	MyModel.SetColorKs(RGB(232, 99, 99));
	MyModel.SetShininess(76.8f);
	AddModel(MyModel);


	// Set a model's transformation matrix
	Matrix model_transform, transform;
	// Initialise transformations to Identity
	model_transform.SetIdentity();
	transform.SetIdentity();

	// Transform the first model that you have in the scene
	TransformModel(0, model_transform);

	transform.SetRotationY(-90);
	model_transform = transform * model_transform;

	transform.SetTranslate(20, 0, 0);
	model_transform = transform * model_transform;

	// Transform the first model that you have in the scene
	TransformModel(1, model_transform);

	// Set the whole client area of the window as the screen viewport
	RECT clientRect;
	GetClientRect(_hWnd, &clientRect);
	SetViewport(clientRect.left, clientRect.right, clientRect.top, clientRect.bottom);

	// Set a specific client area of the window as the screen viewport
	//SetViewport(50, 50 + 600, 50, 50 + 600);

	// Set the camera matrix
	_camRx = 0.0f;
	_camRy = 0.0f;
	_camRz = 0.0f;
	_camZ = 50.0f;
	Matrix camera;
	camera.SetCamera(_camRx, _camRy, _camRz, 0, 0, _camZ);
	SetCameraTransform(camera);

	// Set the projection matrix
	_ortho = false;
	Matrix projection;
	if (_ortho)
	{
		projection.SetOrtho(6.0f);
	}
	else
	{
		projection.SetPerspective(200.0f, 1.0f);
	}

	// Set the viewvolume matrix
	Matrix volume;
	volume.SetViewvolume(-200, +200, -200, +200, +200, -200);

	// Set the overall view matrix = viewvolume matrix * projection matrix
	Matrix view;
	view = volume * projection;
	SetProjectionTransform(view);

	_render_mode = 0;
	_depthsort = false;
	_facecull = true;
	_directional = true;
	_light.SetDirectional(_directional);
	_lPos.SetX(-100.0f);
	_lPos.SetY(100.0f);
	_lPos.SetZ(100.0f);
	_light.SetLPos(_lPos);
	_light.SetColorLa(RGB(32, 32, 32));
	_light.SetColorLd(RGB(255, 255, 0));
	_light.SetColorLs(RGB(255, 255, 255));

	_light.SetColorLa(RGB(128, 128, 128));
	_light.SetColorLd(RGB(255, 255, 255));
	_light.SetColorLs(RGB(255, 255, 255));

	SetRenderParameters(0);
}

void Rasteriser::SetRenderParameters(int action)
{
	Matrix camera, projection, volume, view;

	switch (action)
	{
		case  100: // DrawWireFrame
		{
			_render_mode = 0;
			break; 
		}
		case 101: // DrawSolidFill
		{
			_render_mode = 1;
			break;
		}
		case 102: // DrawFlatShading
		{
			_render_mode = 2;
			break;
		}
		case 103: // DrawVertexShading
		{
			_render_mode = 3;
			break;
		}
		case  1: // Ortho Toggle
		{
			_ortho = !_ortho;

			if (_ortho)
			{
				projection.SetOrtho(6.0f);
			}
			else
			{
				projection.SetPerspective(200.0f, 1.0f);
			}

			volume.SetViewvolume(-200, +200, -200, +200, +200, -200);
			view = volume * projection;
			SetProjectionTransform(view);
			break;
		}
		case  2: // Face Culling Toggle
		{
			_facecull = !_facecull;
			break;
		}
		case  3: // Depth Sort Toggle
		{
			_depthsort = !_depthsort;
			break;
		}
		case  4: // Directional Light Toggle
		{
			_directional = !_directional;
			_light.SetDirectional(_directional);
			break;
		}
		case 5:	// Reset Camera
		{
			_camRx = 0.0f;
			_camRy = 0.0f;
			_camRz = 0.0f;
			_camZ = 50.0f;
			camera.SetCamera(_camRx, _camRy, _camRz, 0, 0, _camZ);
			SetCameraTransform(camera);
			break;
		}
		case 6:	// Rotate + Camera X Axis
		{
			_camRx = (int)(_camRx + 1.0f) % 360;
			camera.SetCamera(_camRx, _camRy, _camRz, 0, 0, _camZ);
			SetCameraTransform(camera);
			break;
		}
		case 7:	// Rotate - Camera X Axis
		{
			_camRx = (int)(_camRx - 1.0f) % 360;
			camera.SetCamera(_camRx, _camRy, _camRz, 0, 0, _camZ);
			SetCameraTransform(camera);
			break;
		}
		case 8:	// Rotate + Camera Y Axis
		{
			_camRy = (int)(_camRy + 1.0f) % 360;
			camera.SetCamera(_camRx, _camRy, _camRz, 0, 0, _camZ);
			SetCameraTransform(camera);
			break;
		}
		case 9:	// Rotate - Camera Y Axis
		{
			_camRy = (int)(_camRy - 1.0f) % 360;
			camera.SetCamera(_camRx, _camRy, _camRz, 0, 0, _camZ);
			SetCameraTransform(camera);
			break;
		}
		case 10: // Rotate + Camera Z Axis
		{
			_camRz = (int)(_camRz + 1.0f) % 360;
			camera.SetCamera(_camRx, _camRy, _camRz, 0, 0, _camZ);
			SetCameraTransform(camera);
			break;
		}
		case 11: // Rotate - Camera Z Axis	
		{
			_camRz = (int)(_camRz - 1.0f) % 360;
			camera.SetCamera(_camRx, _camRy, _camRz, 0, 0, _camZ);
			SetCameraTransform(camera);
			break;
		}
		case 12: // Translate Camera by +10 on Z Axis
		{
			_camZ = _camZ + 10.0f;
			camera.SetCamera(_camRx, _camRy, _camRz, 0, 0, _camZ);
			SetCameraTransform(camera);
			break;
		}
		case 13: // Translate Camera by -10 on Z Axis
		{
			_camZ = _camZ - 10.0f;
			camera.SetCamera(_camRx, _camRy, _camRz, 0, 0, _camZ);
			SetCameraTransform(camera);
			break;
		}
		case 14: // Scale Down Model 
		{
			// Set a model's transformation matrix
			Matrix model_transform, transform;
			// Initialise transformations to Identity
			model_transform.SetIdentity();
			transform.SetIdentity();

			transform.SetScale(0.996f, 0.996f, 0.996f);
			model_transform = transform * model_transform;

			// Transform the first model that you have in the scene
			TransformModel(0, model_transform);
			break;
		}
		case 15: // Scale Up Model 
		{
			// Set a model's transformation matrix
			Matrix model_transform, transform;
			// Initialise transformations to Identity
			model_transform.SetIdentity();
			transform.SetIdentity();

			transform.SetScale(1.003f, 1.003f, 1.003f);
			model_transform = transform * model_transform;

			// Transform the first model that you have in the scene
			TransformModel(0, model_transform);
			break;
		}
		case 16: // Rotate + Model X Axis
		{
			// Set a model's transformation matrix
			Matrix model_transform, transform;
			// Initialise transformations to Identity
			model_transform.SetIdentity();
			transform.SetIdentity();

			transform.SetRotationX(1);
			model_transform = transform * model_transform;

			// Transform the first model that you have in the scene
			TransformModel(0, model_transform);
			break;
		}
		case 17: // Rotate - Model X Axis 
		{
			// Set a model's transformation matrix
			Matrix model_transform, transform;
			// Initialise transformations to Identity
			model_transform.SetIdentity();
			transform.SetIdentity();

			transform.SetRotationX(-1);
			model_transform = transform * model_transform;

			// Transform the first model that you have in the scene
			TransformModel(0, model_transform);
			break;
		}
		case 18: // Rotate + Model Y Axis
		{
			// Set a model's transformation matrix
			Matrix model_transform, transform;
			// Initialise transformations to Identity
			model_transform.SetIdentity();
			transform.SetIdentity();

			transform.SetRotationY(1);
			model_transform = transform * model_transform;

			// Transform the first model that you have in the scene
			TransformModel(0, model_transform);
			break;
		}
		case 19: // Rotate - Model Y Axis 
		{
			// Set a model's transformation matrix
			Matrix model_transform, transform;
			// Initialise transformations to Identity
			model_transform.SetIdentity();
			transform.SetIdentity();

			transform.SetRotationY(-1);
			model_transform = transform * model_transform;

			// Transform the first model that you have in the scene
			TransformModel(0, model_transform);
			break;
		}
		case 20: // Rotate + Model Z Axis
		{
			// Set a model's transformation matrix
			Matrix model_transform, transform;
			// Initialise transformations to Identity
			model_transform.SetIdentity();
			transform.SetIdentity();

			transform.SetRotationZ(1);
			model_transform = transform * model_transform;

			// Transform the first model that you have in the scene
			TransformModel(0, model_transform);
			break;
		}
		case 21: // Rotate - Model Z Axis 
		{
			// Set a model's transformation matrix
			Matrix model_transform, transform;
			// Initialise transformations to Identity
			model_transform.SetIdentity();
			transform.SetIdentity();

			transform.SetRotationZ(-1);
			model_transform = transform * model_transform;

			// Transform the first model that you have in the scene
			TransformModel(0, model_transform);
			break;
		}
		case 22: // Translate + Model X Axis 
		{
			// Set a model's transformation matrix
			Matrix model_transform, transform;
			// Initialise transformations to Identity
			model_transform.SetIdentity();
			transform.SetIdentity();

			transform.SetTranslate(1.0f, 0.0f, 0.0f);
			model_transform = transform * model_transform;

			// Transform the first model that you have in the scene
			TransformModel(0, model_transform);
			break;
		}
		case 23: // Translate - Model X Axis 
		{
			// Set a model's transformation matrix
			Matrix model_transform, transform;
			// Initialise transformations to Identity
			model_transform.SetIdentity();
			transform.SetIdentity();

			transform.SetTranslate(-1.0f, 0.0f, 0.0f);
			model_transform = transform * model_transform;

			// Transform the first model that you have in the scene
			TransformModel(0, model_transform);
			break;
		}
		case 24: // Translate + Model Y Axis 
		{
			// Set a model's transformation matrix
			Matrix model_transform, transform;
			// Initialise transformations to Identity
			model_transform.SetIdentity();
			transform.SetIdentity();

			transform.SetTranslate(0.0f, 1.0f, 0.0f);
			model_transform = transform * model_transform;

			// Transform the first model that you have in the scene
			TransformModel(0, model_transform);
			break;
		}
		case 25: // Translate - Model Y Axis 
		{
			// Set a model's transformation matrix
			Matrix model_transform, transform;
			// Initialise transformations to Identity
			model_transform.SetIdentity();
			transform.SetIdentity();

			transform.SetTranslate(0.0f, -1.0f, 0.0f);
			model_transform = transform * model_transform;

			// Transform the first model that you have in the scene
			TransformModel(0, model_transform);
			break;
		}
		case 26: // Translate + Model Z Axis 
		{
			// Set a model's transformation matrix
			Matrix model_transform, transform;
			// Initialise transformations to Identity
			model_transform.SetIdentity();
			transform.SetIdentity();

			transform.SetTranslate(0.0f, 0.0f, 1.0f);
			model_transform = transform * model_transform;

			// Transform the first model that you have in the scene
			TransformModel(0, model_transform);
			break;
		}
		case 27: // Translate - Model Z Axis 
		{
			// Set a model's transformation matrix
			Matrix model_transform, transform;
			// Initialise transformations to Identity
			model_transform.SetIdentity();
			transform.SetIdentity();

			transform.SetTranslate(0.0f, 0.0f, -1.0f);
			model_transform = transform * model_transform;

			// Transform the first model that you have in the scene
			TransformModel(0, model_transform);
			break;
		}
		
	}
}

void Rasteriser::Update(Bitmap& bitmap)
{
	// Changes action every 5 seconds
	if (clock() < 1 * _timeFactor)
	{
		SetRenderParameters(16);
		DrawString(bitmap, L"Rotation on X axis.");
	}
	else if (clock() > 1 * _timeFactor && clock() <= 2 * _timeFactor)
	{
		SetRenderParameters(18);
		DrawString(bitmap, L"Rotation on Y axis.");
	}
	else if (clock() > 2 * _timeFactor && clock() <= 3 * _timeFactor)
	{
		SetRenderParameters(20);
		DrawString(bitmap, L"Rotation on Z axis.");
	}
	else if (clock() > 3 * _timeFactor && clock() <= 4 * _timeFactor)
	{
		SetRenderParameters(17);
		SetRenderParameters(19);
		SetRenderParameters(21);
		DrawString(bitmap, L"Rotation on all axis.");
	}
	else if (clock() > 4 * _timeFactor && clock() <= 4.5 * _timeFactor)
	{
		SetRenderParameters(14);
		DrawString(bitmap, L"Scale Down Model.");
	}
	else if (clock() > 4.5 * _timeFactor && clock() <= 5 * _timeFactor)
	{
		SetRenderParameters(15);
		DrawString(bitmap, L"Scale Up Model.");
	}
	else if (clock() > 5 * _timeFactor && clock() <= 6 * _timeFactor)
	{
		// Sets Depth Sort and Face Culling only once to avoid flickering
		if (counter == 0)
		{
			SetRenderParameters(2);
			SetRenderParameters(3);
			counter++;
		}

		SetRenderParameters(16);
		SetRenderParameters(18);
		SetRenderParameters(18);
		SetRenderParameters(20);
		DrawString(bitmap, L"Depth Sort and Face Culling Enabled.");
	}
	else if (clock() > 6 * _timeFactor && clock() <= 7 * _timeFactor)
	{
		// Sets Solid Fill only once to avoid flickering
		if (counter == 1)
		{
			SetRenderParameters(101);
			counter++;
		}
		
		SetRenderParameters(16);
		SetRenderParameters(18);
		SetRenderParameters(18);
		SetRenderParameters(20);
		DrawString(bitmap, L"Set Solid Fill.");
	}
	else if (clock() > 7 * _timeFactor && clock() <= 9 * _timeFactor)
	{
		// Sets Solid Fill only once to avoid flickering
		if (counter == 2)
		{
			SetRenderParameters(102);
			counter++;
		}

		SetRenderParameters(16);
		SetRenderParameters(18);
		SetRenderParameters(18);
		SetRenderParameters(20);
		DrawString(bitmap, L"Set Flat Shading.");
	}
	else if (clock() > 9 * _timeFactor && clock() <= 9.1 * _timeFactor)
	{
		// Sets Solid Fill only once to avoid flickering
		if (counter == 3)
		{
			SetRenderParameters(103);
			counter++;
		}

		SetRenderParameters(16);
		SetRenderParameters(18);
		SetRenderParameters(18);
		SetRenderParameters(20);
		DrawString(bitmap, L"Set Vector Shading.");
	}
	else if (clock() > 9.1 * _timeFactor && clock() <= 20 * _timeFactor)
	{
		SetRenderParameters(16);
		SetRenderParameters(18);
		SetRenderParameters(18);
		SetRenderParameters(20);
		DrawString(bitmap, L"Set Vector Shading.");
	}
}

