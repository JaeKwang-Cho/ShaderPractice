#include "d3_init.h"

// D3D 관련
LPDIRECT3D9             gpD3D = NULL;				// D3D
LPDIRECT3DDEVICE9       gpD3DDevice = NULL;				// D3D 장치

const char* gAppName = "베이스 코드";

//-----------------------------------------------------------------------
// 프로그램 진입점/메시지 루프
//-----------------------------------------------------------------------

// 진입점
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	// 1. 윈도우 클래스를 등록한다.
	// 프로그램이 창을 생성하기 위해서는 제일 먼저 윈도우 클래스를 등록해야한다.
	// 1) 클래스 크기, 2) 스타일 3) 윈도우 프로시져 포인터 4) 클래스 추가 메모리 5) 윈도우 추가메모리
	// 6) 핸들러 7) 아이콘 리소스 포인터 8) 커서 리소스 포인터 9) 브러쉬 핸들 10) 메뉴 이름 포인터
	// 11) 클래스 이름 포인터 12) 아이콘 포인터
	// 뭐... 정 하나하나가 궁금하면 찾아보자
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L,
					  GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
					  gAppName, NULL };
	RegisterClassEx(&wc);

	// 2. 프로그램 창을 생성한다.
	// 윈도우 클래스의 인스턴스를 생성한다.
	// 역시 어떤 인자를 넘기는지 궁금하면 찾아보자
	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	HWND hWnd = CreateWindow(gAppName, gAppName,
		style, CW_USEDEFAULT, 0, WIN_WIDTH, WIN_HEIGHT,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	// 3. Client Rect 크기가 WIN_WIDTH, WIN_HEIGHT와 같도록 크기를 조정한다.
	POINT ptDiff;
	RECT rcClient, rcWindow;

	// 실제 렌더링이 가능한 공간이 Client Rect이다.
	GetClientRect(hWnd, &rcClient);
	GetWindowRect(hWnd, &rcWindow);
	// 윈도우 틀로 짤리는 부분의 좌표를 구하고
	ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
	ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
	// 조정한다.
	MoveWindow(hWnd, rcWindow.left, rcWindow.top, WIN_WIDTH + ptDiff.x, WIN_HEIGHT + ptDiff.y, TRUE);

	// 4. 이제 창을 보여준다.
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);

	// 5. D3D를 비롯한 모든 것을 초기화한다.
	if (!InitEverything(hWnd))
		PostQuitMessage(1);

	// 6. 메시지 루프
	MSG msg;
	// 일종의 메크로 함수 메모리를 0을 초기화
	ZeroMemory(&msg, sizeof(msg));
	// WM_QUIT 받기 전까지 계속 돌린다.
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

	// 이것도 제공 메크로함수이고, 윈도우 클래스의 등록을 해제한다.
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
