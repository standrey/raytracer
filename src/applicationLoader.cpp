#include "global.h"
#include "directx.h"
#include "stScene.h"

LPCWSTR szClassName = L"RayTracerClass";
LPCWSTR szAppName = L"RayTracerApp";
LPCWSTR szTitle = L"RayTracer";

static stScene * pGeneralScene = NULL;

LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
		pGeneralScene->KeyPress(wParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return CallWindowProc((WNDPROC)DefWindowProc, hWnd, msg, wParam, lParam);
}

INT __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	MSG			uMsg;
	WNDCLASS	wndclass;
	extern HWND hWnd;

	memset(&uMsg,0,sizeof(uMsg));
	
	wndclass.style         = 0;
	wndclass.lpfnWndProc   = (WNDPROC)WndProc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 0;
	wndclass.hInstance     = hInstance;
	wndclass.hIcon         = LoadIcon (hInstance, L"RayTracerIcon");
	wndclass.hCursor       = LoadCursor (NULL,IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wndclass.lpszMenuName  = L"RayTracerMenu";
	wndclass.lpszClassName = szClassName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_OK | MB_ICONEXCLAMATION);
		return GetLastError();
	}

	DWORD dwStyle = WS_OVERLAPPEDWINDOW;
	DWORD nWidth = 600;
	DWORD nHeight = 600;

	hWnd = CreateWindow(szClassName, szTitle, dwStyle, 0, 0, nWidth, nHeight, 0, 0, hInstance, NULL);

	if (hWnd == NULL)
	{
		MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		return GetLastError();
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	DirectX pDx;
	if(FAILED(pDx.Init(hWnd)))
	{
		MessageBox(NULL, L"Can't Initialise Direct3D9", L"Error!", MB_OK | MB_ICONSTOP);
		UnregisterClass(szClassName, wndclass.hInstance );
		return uMsg.wParam;
	}

	MSG mssg;
	pGeneralScene = new stScene(pDx, nWidth, nHeight); 
	PeekMessage( &mssg, NULL, 0, 0, PM_NOREMOVE);
	while (mssg.message!=WM_QUIT) 
	{
		if (PeekMessage(&mssg, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&mssg);
			DispatchMessage(&mssg);
		} 
		else 
		{
			pGeneralScene->UpdateObjects();
			pGeneralScene->DrawObjects();
			Sleep(1);
		}
	}
	delete pGeneralScene;
	UnregisterClass(szClassName, wndclass.hInstance );
	return uMsg.wParam;
}