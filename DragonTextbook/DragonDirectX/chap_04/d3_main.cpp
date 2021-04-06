#include "d3_lighting.h"


LPDIRECT3D9             gpD3D = NULL;				// D3D
LPDIRECT3DDEVICE9       gpD3DDevice = NULL;				// D3D 장치

const char* gAppName = "조명";

//-----------------------------------------------------------------------
// 프로그램 진입점/메시지 루프
//-----------------------------------------------------------------------


INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L,
					  GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
					  gAppName, NULL };
	RegisterClassEx(&wc);


	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	HWND hWnd = CreateWindow(gAppName, gAppName,
		style, CW_USEDEFAULT, 0, WIN_WIDTH, WIN_HEIGHT,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	POINT ptDiff;
	RECT rcClient, rcWindow;

	GetClientRect(hWnd, &rcClient);
	GetWindowRect(hWnd, &rcWindow);
	
	ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
	ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
	
	MoveWindow(hWnd, rcWindow.left, rcWindow.top, WIN_WIDTH + ptDiff.x, WIN_HEIGHT + ptDiff.y, TRUE);

	
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);

	
	if (!InitEverything(hWnd))
		PostQuitMessage(1);

	
	MSG msg;
	
	ZeroMemory(&msg, sizeof(msg));
	
	static float lastTime = (float)timeGetTime();
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			float currTime = (float)timeGetTime();
			float timeDelta = (currTime - lastTime) * 0.001f;

			Display(timeDelta);

			lastTime = currTime;
		}
	}

	
	UnregisterClass(gAppName, wc.hInstance);
	return 0;
}


LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		ProcessInput(hWnd, wParam);
		break;

	case WM_DESTROY:
		Cleanup();
		PostQuitMessage(0);
		return 0;
	}
	
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


void ProcessInput(HWND hWnd, WPARAM keyPress)
{
	switch (keyPress)
	{
		
	case VK_ESCAPE:
		
		PostMessage(hWnd, WM_DESTROY, 0L, 0L);
		break;
	}
}