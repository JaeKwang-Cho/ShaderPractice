#include "d3_stencil.h"


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
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

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
	// 1. 물리 디스플레이 어뎁터	2.이용할 장치 타입 설정 3. 윈도우 핸들, 보통 장치가 드로잉을 수행할 윈도우
	// 4. 하드웨어 혹은 소프트웨어 프로세싱 5. 장치 특성의 일부를 정의하는 초기화된 인스턴스
	// 6. 생성된 장치 리턴
	if (FAILED(gpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &gpD3DDevice)))
	{
		return false;
	}

	return true;

}

D3DMATERIAL9 d3d::InitMtrl(D3DXCOLOR ambient, D3DXCOLOR diffuse, D3DXCOLOR specular, D3DXCOLOR emissive, float power)
{
	D3DMATERIAL9 mtrl;

	mtrl.Ambient = ambient;
	mtrl.Diffuse = diffuse;
	mtrl.Specular = specular;
	mtrl.Emissive = emissive;
	mtrl.Power = power;

	return mtrl;
}
D3DLIGHT9 d3d::InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = *color * 0.4f;
	light.Diffuse = *color;
	light.Specular = *color * 0.6f;
	light.Direction = *direction;

	return light;
}
D3DLIGHT9 d3d::InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type = D3DLIGHT_POINT;
	light.Ambient = *color * 0.4f;
	light.Diffuse = *color;
	light.Specular = *color * 0.6f;
	light.Position = *position;

	return light;
}
D3DLIGHT9 d3d::InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type = D3DLIGHT_SPOT;
	light.Ambient = *color * 0.4f;
	light.Diffuse = *color;
	light.Specular = *color * 0.6f;
	light.Direction = *direction;
	light.Position = *position;

	return light;
}