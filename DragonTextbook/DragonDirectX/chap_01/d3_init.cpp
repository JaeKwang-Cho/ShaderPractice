#include "d3_init.h"

// D3D 관련
LPDIRECT3D9             gpD3D = NULL;				// D3D
LPDIRECT3DDEVICE9       gpD3DDevice = NULL;				// D3D 장치

const char* gAppName = "컬러 쉐이더 프레임워크";

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
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
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
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else // 메시지가 없으면 게임을 업데이트하고 장면을 그린다
		{
		}
	}
	// 이것도 제공 메크로함수이고, 윈도우 클래스의 등록을 해제한다.
	UnregisterClass(gAppName, wc.hInstance);
	return 0;
}

// 메시지 처리기
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

//------------------------------------------------------------
// 초기화 코드
//------------------------------------------------------------
bool InitEverything(HWND hWnd)
{
	// D3D를 초기화
	if (!InitD3D(hWnd))
	{
		return false;
	}
	return true;
}

// D3D 객체 및 장치 초기화
bool InitD3D(HWND hWnd)
{
	// D3D 초기화는 항상 이렇게 시작한다.
	gpD3D = Direct3DCreate9(D3D_SDK_VERSION);
	// 1. 장치 검증 2. IDirect3DDevice9 개체 생성
	// + 올바른 헤더 파일로 만들어 졌는지 확인
	if (!gpD3D)
	{
		return false;
	}

	// 기본 디스플레이 어뎁터의 특성으로 D3DCAPS9 구조체를 채운다.
	D3DCAPS9 caps;
	gpD3D->GetDeviceCaps(
		D3DADAPTER_DEFAULT, // 기본 디스플레이 어뎁터를 표기한다.
		D3DDEVTYPE_HAL,		// 장치 타입을 지정한다. 
		&caps				// 기본 디스플레이 어뎁터의 특성으로 채워진다.
	);

	int vp = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		// 하드웨어 버텍스 프로세싱이 지원된다.
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		// 소프트웨어 버텍스 프로세싱을 이용해야 한다.
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	// D3Device9을 초기화하는데 필요한 구조체를 채워넣는다.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.BackBufferWidth = WIN_WIDTH;	// 렌더링 영역, 백버퍼의 너비 
	d3dpp.BackBufferHeight = WIN_HEIGHT;	// 백버퍼의 높이 (픽셀 단위이다.)
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;// 백버퍼의 포멧 (몇 비트 포맷)
	d3dpp.BackBufferCount = 1;			// 보통 한개의 후면 버퍼를 이용한다.
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;	// 멀티 셈플링 레벨, 궁금하면 찾아보자.
	d3dpp.MultiSampleQuality = 0;		// 멀티 샘플링 레벨
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;// 백버퍼를 스왑할 때의 효과. 성능상 이게 좋다.
	d3dpp.hDeviceWindow = hWnd;	// 
	d3dpp.Windowed = TRUE;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8; // 깊이/스텐실 버퍼의 포맷
	d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // 모니터 주사율과 백버퍼를 스왑하는 빈도간의 관계. 이것은 모니터가 수직 동기될 때마다 백버퍼를 스왑해준다.
	// 게임에서는 성능상 모니터의 수직 동기를 기다리지 않고 곧바로 스왑해 주는 경우가 많다.(D3DPRESENT IMMEDIATE) 단, 이럴 때는 전 프레임과 현재 프레임이 서로 찢겨보이는 부작용이 있다.

	// 위 정보(D3DPRESENT_PARAMETERS 구조체)를 가지고 D3D장치를 생성한다.
	if (FAILED(gpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &gpD3DDevice)))
	{
		return false;
	}

	return true;
}

void Cleanup()
{
	// 마지막으로 D3D를 release 한다.
	if (gpD3DDevice)
	{
		gpD3DDevice->Release();
		gpD3DDevice = NULL;
	}

	if (gpD3D)
	{
		gpD3D->Release();
		gpD3D = NULL;
	}
}
