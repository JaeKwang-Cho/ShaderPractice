#include "d3_init.h"

// D3D 관련
extern LPDIRECT3D9             gpD3D;				// D3D
extern LPDIRECT3DDEVICE9       gpD3DDevice;				// D3D 장치

//------------------------------------------------------------
// 초기화 코드
//------------------------------------------------------------
bool InitEverything(HWND hWnd)
{
	// D3D를 초기화
	if (!InitD3D(hWnd))
	{		
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
			
		return false;
	}
	if (!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
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
bool Setup()
{
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

