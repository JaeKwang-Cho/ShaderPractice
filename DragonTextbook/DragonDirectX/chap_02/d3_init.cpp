#include "d3_draw.h"

// D3D 관련
LPDIRECT3D9             gpD3D = NULL;				// D3D
LPDIRECT3DDEVICE9       gpD3DDevice = NULL;				// D3D 장치

// 입방체 버텍스, 인덱스 
IDirect3DVertexBuffer9* VB = NULL;
IDirect3DIndexBuffer9* IB = NULL;



const char* gAppName = "간단한 메쉬 그려보기";

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
		// 입방체를 회전 시킨다.
		D3DXMATRIX Rx, Ry;

		// x 축으로 45도 회전 시킨다.
		D3DXMatrixRotationX(&Rx, 3.14f / 4.0f);

		// 각 프레임당 y 회전을 증가시킨다.
		static float y = 0.0f;
		D3DXMatrixRotationY(&Ry, y);
		y += timeDelta;

		// 각도가 2*PI에 이르면 0으로 초기화 한다.
		if (y >= 6.28f)
		{
			y = 0.0f;
		}
		// 회전을 결합한다.
		D3DXMATRIX p = Rx * Ry;

		gpD3DDevice->SetTransform(D3DTS_WORLD, &p);

		// 장면을 그린다.
		gpD3DDevice->Clear(0, 0,
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			0xffffffff, 1.0f, 0);
		gpD3DDevice->BeginScene();

		gpD3DDevice->SetStreamSource(0, VB, 0, sizeof(Vertex));
		gpD3DDevice->SetIndices(IB);
		gpD3DDevice->SetFVF(Vertex::FVF);
		gpD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
			0, 0, 8, 0, 12);
		// 1. 기본도형 2.인덱스 버퍼 시작 지점 3.	정점 버퍼 시작 지점 4. 정점의 수
		//  5. 인덱스 시작할때 정점이 시작하는 위치 6. 기본 도형의 수


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
