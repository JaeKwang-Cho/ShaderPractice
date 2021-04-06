#include "d3_color.h"

// D3D 관련
LPDIRECT3D9             gpD3D = NULL;				// D3D
LPDIRECT3DDEVICE9       gpD3DDevice = NULL;				// D3D 장치

// 월드 변환과 삼각형 버텍스데이터
D3DXMATRIX				World;
IDirect3DVertexBuffer9* Triangle = NULL;



const char* gAppName = "색칠해보기";

//-----------------------------------------------------------------------
// 프로그램 진입점/메시지 루프
//-----------------------------------------------------------------------

// 진입점
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

// 메시지 처리기
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
	// 처리하지 않는 메시지를 default 기본 처리
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// 키보드 입력처리
void ProcessInput(HWND hWnd, WPARAM keyPress)
{
	switch (keyPress)
	{
		// ESC 키가 눌리면 프로그램을 종료한다.
	case VK_ESCAPE:
		// 윈도우 핸들에게 메시지를 보내는 메크로함수
		PostMessage(hWnd, WM_DESTROY, 0L, 0L);
		break;
	}
}

bool Display(float timeDelta)
{
	if (gpD3DDevice)
	{
		gpD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			0xffffffff, 1.0f, 0);
		gpD3DDevice->BeginScene();

		gpD3DDevice->SetFVF(ColorVertex::FVF);
		gpD3DDevice->SetStreamSource(0, Triangle, 0, sizeof(ColorVertex));

		// 플랫 셰이딩으로 화면 왼쪽에 삼각형을 그린다.
		D3DXMatrixTranslation(&World, -1.25f, 0.0f, 0.0f);
		gpD3DDevice->SetTransform(D3DTS_WORLD, &World);

		gpD3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
		gpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

		// 그라우드 셰이딩으로 화면 오른쪽에 삼각형을 그린다.
		D3DXMatrixTranslation(&World, 1.25f, 0.0f, 0.0f);
		gpD3DDevice->SetTransform(D3DTS_WORLD, &World);

		gpD3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		gpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);


		gpD3DDevice->EndScene();
		gpD3DDevice->Present(0, 0, 0, 0);

	}
	return true;
}


int EnterMsgLoop(bool (*ptr_display)(float timeDelta))
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	static float lastTime = (float)timeGetTime();

	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			float currTime = (float)timeGetTime();
			float timeDelta = (currTime - lastTime) * 0.001f;

			ptr_display(timeDelta);

			lastTime = currTime;
		}
	}
	return msg.wParam;
}
