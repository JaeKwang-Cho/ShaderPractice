//**********************************************************************
//
// ShaderFramework.cpp
// 
// 쉐이더 데모를 위한 C스타일의 초간단 프레임워크입니다.
// (실제 게임을 코딩하실 때는 절대 이렇게 프레임워크를
// 작성하시면 안됩니다. -_-)
//
// Author: Pope Kim
//
//**********************************************************************

#include "ShaderFramework.h"
#include <stdio.h>

// 투영행렬을 만들 때 필요한 상수들을 일단 정의
#define PI				3.14159265f
#define FOV				(PI/4.0f)						// 시야각
#define ASPECT_RATIO	((WIN_WIDTH)/(float)WIN_HEIGHT)	// 화면 종횡비
#define NEAR_PLANE		1								// 근접 평면
#define FAR_PLANE		10000							// 원거리 평면


//----------------------------------------------------------------------
// 전역변수
//----------------------------------------------------------------------

// D3D 관련
LPDIRECT3D9             gpD3D = NULL;				// D3D
LPDIRECT3DDEVICE9       gpD3DDevice = NULL;			// D3D 장치

// 폰트
ID3DXFont* gpFont = NULL;

// 모델
LPD3DXMESH				gpTorus = NULL;
LPD3DXMESH				gpDisc = NULL;
// 쉐이더
LPD3DXEFFECT			gpApplyShadowShader	= NULL;
LPD3DXEFFECT			gpCreateShadowShader = NULL;
// 텍스처

// 프로그램 이름
const char*				gAppName = "ShadowMapping 쉐이더 프레임워크";
// 회전값
float					gRotationY = 0.0f;
// 빛의 위치
D3DXVECTOR4				gWorldLightPosition(500.0f, 500.0f, -500.0f, 1.0f);
// 카메라 위치
D3DXVECTOR4				gWorldCameraPosition(0.0f, 0.0f, -200.0f, 1.0f);

// 물체의 색상
D3DXVECTOR4				gTorusColor(1, 1, 0, 1);
D3DXVECTOR4				gDiscColor(0, 1, 1, 1);

// 그림자맵 렌더타깃
LPDIRECT3DTEXTURE9		gpShadowRenderTarget = NULL;
LPDIRECT3DSURFACE9		gpShadowDepthStencil = NULL;


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
			PlayDemo();
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
// 게임루프
//------------------------------------------------------------
void PlayDemo()
{
	Update();
	RenderFrame();
}

// 게임로직 업데이트
void Update()
{
}

//------------------------------------------------------------
// 렌더링
//------------------------------------------------------------

void RenderFrame()
{
	D3DCOLOR bgColour = 0x00000000;	// 배경색상 - 파랑

	// 깔끔하게 색만 남기고, 깊이/스텐실을 clear 한다.
	gpD3DDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), bgColour, 1.0f, 0);

	// 씬을 그리기 시작한다.
	gpD3DDevice->BeginScene();
	{
		RenderScene();				// 3D 물체등을 그린다.
		RenderInfo();				// 디버그 정보 등을 출력한다.
	}
	// 씬을 다 그렸으면
	gpD3DDevice->EndScene();

	// 백버퍼에 있는 씬을 화면에 뿌린다.
	gpD3DDevice->Present(NULL, NULL, NULL, NULL);
}


// 3D 물체등을 그린다.
void RenderScene()
{
	// 광원 뷰 행렬을 만든다.
	D3DXMATRIXA16	matLightView;
	{
		// 빛의 위치 (사실은 시점의 위치)
		D3DXVECTOR3 vEyePt(gWorldLightPosition.x, gWorldLightPosition.y, gWorldLightPosition.z);
		// 빛은 원점을 바라본다. 
		D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
		// 필드의 위쪽을 정의하는 벡터
		D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
		// 1) 결과 포인터 2) 카메라 벡터 3) 바라보는 방향 벡터 4) 필드의 위쪽을 가리키는 벡터
		D3DXMatrixLookAtLH(&matLightView, &vEyePt, &vLookatPt, &vUpVec);
	}
	// 광원 투영 행렬을 만든다.
	D3DXMATRIXA16 matLightProjection;
	{
		// 1)결과 포인터 2) y 방향의 시야(라디안) 3) 가로세로 비율 4) 가까운 뷰 평면의 z 값 5) 먼 뷰 평면의 z 값
		D3DXMatrixPerspectiveFovLH(&matLightProjection, D3DX_PI / 4.0f, 1, 1, 3000);
	}
	// 뷰/튜영 행렬을 만든다.
	D3DXMATRIXA16 matViewProjection;
	{
		// 뷰 행렬을 만든다.
		D3DXMATRIXA16 matView;
		// 카메라 위치
		D3DXVECTOR3 vEyePt(gWorldCameraPosition.x, gWorldCameraPosition.y, gWorldCameraPosition.z);
		// 카메라가 바라보는 곳
		D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
		// 필드의 위쪽 방향 벡터
		D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
		// 1) 결과 포인터 2) 카메라 위치 3) 카메라가 바로보는 곳 4) 필드의 위쪽 방향 벡터
		D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);

		// 투영행렬을 만든다.
		D3DXMATRIXA16		 matProjection;
		// 1) 결과 포인터 2) y 방향의 시야(라디안) 3) 화면 종횡비 4) 가까운 뷰 평면의 z 값 5) 먼 뷰평면의 z 값
		D3DXMatrixPerspectiveFovLH(&matProjection, FOV, ASPECT_RATIO, NEAR_PLANE, FAR_PLANE);

		// 1) 결과 포인터 = A*B 2) A 3) B 
		D3DXMatrixMultiply(&matViewProjection, &matView, &matProjection);
	}
	// 원환체의 월드 행렬을 만든다.
	D3DXMATRIXA16 matTorusWorld;
	{
		//프레임 마다 0.4도씩 회전을 시킨다.
		gRotationY += 0.4f * PI / 180.0f;
		if (gRotationY > 2 * PI)
		{
			gRotationY -= 2 * PI;
		}
		// 주어진 값만큼 돌리는 함수
		D3DXMatrixRotationY(&matTorusWorld, gRotationY);
	}
	// 디스크의 월드 행렬을 만든다.
	D3DXMATRIXA16		 matDiscWorld;
	{
		// 확대할 때 이용하는 행렬
		D3DXMATRIXA16 matScale;
		D3DXMatrixScaling(&matScale, 2, 2, 2);

		// 평행이동 할때 이용하는 행렬
		D3DXMATRIXA16 matTrans;
		D3DXMatrixTranslation(&matTrans, 0, -40, 0);

		D3DXMatrixMultiply(&matDiscWorld, &matScale, &matTrans);
	}

	/* 이제 렌더링을 한다*/
	// 1. 그림자를 만든다.
	//	- 하드웨어 백버퍼 대신에 그림자 맵을 렌더타깃으로 설정하고
	//	- gpShadowShader로 원환체를 그린다.
	// 2. 일반적인 렌더링을 하는데 그위에 그림자를 씌운다.
	//	- 하드웨어 백버퍼를 렌더 타깃으로 설정하고, 원환체와 디스크를 그린다.
	// (이때 위치와 색이 다르므로 그에 맞게 셰이더 변수도 지정해준다.)

	// 현재 하드웨어 백버퍼와 깊이버퍼
	LPDIRECT3DSURFACE9 pHWBackBuffer = NULL;
	LPDIRECT3DSURFACE9 pHWDepthStencilBuffer = NULL;
	// 렌더 타겟을 얻을 수 있는, D3D 장치 제공함수 (첫번째 인자는 인덱스)
	gpD3DDevice->GetRenderTarget(0, &pHWBackBuffer);
	// 깊이 버퍼의 포인터를 구할 수 있는 D3D 장치 제공함수
	gpD3DDevice->GetDepthStencilSurface(&pHWDepthStencilBuffer);

	/* 1. 그림자 만들기 */
	// 그림차맵의 렌더 타겟과 깊이 버퍼를 사용한다.
	LPDIRECT3DSURFACE9 pShadowSurface = NULL;
	// 인자로 SURFACE를 받는 TEXTURE 제공함수
	// 렌더 타겟에서 첫번째 밉맵을 가져와서 렌더 타겟으로 넣어주고
	if (SUCCEEDED(gpShadowRenderTarget->GetSurfaceLevel(0, &pShadowSurface)))
	{
		// 그것을 세팅한다. (첫번째 인자는 인덱스)
		gpD3DDevice->SetRenderTarget(0, pShadowSurface);
		pShadowSurface->Release();
		pShadowSurface = NULL;
	}
	// 그리고 깊이 버퍼를 세팅한다.
	gpD3DDevice->SetDepthStencilSurface(gpShadowDepthStencil);

	//저번 프레임에 그렸던 그림자 정보를 지움
	// 1. 두번째 인자의 요소 갯수 2. D3DRECT 배열 포인터 3. 플래그 (스텐실, 렌더링, 깊이버퍼) 4. 색감 5. 새로운 z 값 6. 스텐실 버퍼의 비트값
	gpD3DDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), 0xFFFFFFFF, 1.0f, 0);

	// 그림자 만들기 셰이더 전역변수들을 설정
	gpCreateShadowShader->SetMatrix("gWorldMatrix", &matTorusWorld);
	gpCreateShadowShader->SetMatrix("gLightViewMatrix", &matLightView);
	gpCreateShadowShader->SetMatrix("gLightProjectionMatrix", &matLightProjection);

	// 그림자 만들기 셰이더 시작
	{
		UINT	numPasses = 0;
		gpCreateShadowShader->Begin(&numPasses, NULL);
		{
			for (UINT i = 0; i < numPasses; ++i)
			{
				gpCreateShadowShader->BeginPass(i);
				{
					// 원환체 를 그린다.
					gpTorus->DrawSubset(0);
				}
				gpCreateShadowShader->EndPass();
			}
		}
		gpCreateShadowShader->End();
	}

	/* 2. 그림자 입히기 */
	// 하드웨어 백버퍼/깊이 버퍼를 사용한다.
	gpD3DDevice->SetRenderTarget(0, pHWBackBuffer);
	gpD3DDevice->SetDepthStencilSurface(pHWDepthStencilBuffer);

	// 이렇게 메모리를 해제해주는 이유는
	// GetRenderTarget()이나 GetDepthStencilSuface()를 통해 SURFACE를 얻어올때
	// D3D가 내부적으로 이 버퍼들의 ref count를 증가시켜준다. 그래서 GPU 메모리 누수가 일어나게 된다. 
	// (D3D는 ref count를 보고 메모리를 해제해준다.)
	pHWBackBuffer->Release();
	pHWBackBuffer = NULL;
	pHWDepthStencilBuffer->Release();
	pHWDepthStencilBuffer = NULL;

	// 그림자 입히기 셰이더 전역변수들을 설정
	// 일번적으로 렌더링에 필요한 월드/뷰/투영 행렬
	gpApplyShadowShader->SetMatrix("gWorldMatrix", &matTorusWorld); // 원환체
	gpApplyShadowShader->SetMatrix("gViewProjectionMatrix", &matViewProjection);

	// 광원으로 부터 물체까지의 깊이를 구하기 위한 광원-뷰행렬 / 광원-투영행렬
	gpApplyShadowShader->SetMatrix("gLightViewMatrix", &matLightView);
	gpApplyShadowShader->SetMatrix("gLightProjectionMatrix", &matLightProjection);

	// 난반사광 계산을 위한 광원의 위치
	gpApplyShadowShader->SetVector("gWorldLightPosition", &gWorldLightPosition);

	// 물체의 색
	gpApplyShadowShader->SetVector("gObjectColor", &gTorusColor);

	// 그림자 맵전달
	gpApplyShadowShader->SetTexture("ShadowMap_Tex", gpShadowRenderTarget);

	// 셰이더 시작
	UINT numPasses = 0;
	gpApplyShadowShader->Begin(&numPasses, NULL);
	{
		for (UINT i = 0; i < numPasses; ++i)
		{
			gpApplyShadowShader->BeginPass(i);
			{
				// 원환체를 그린다.
				gpTorus->DrawSubset(0);
				// 디스크를 그린다.
				gpApplyShadowShader->SetMatrix("gWorldMatrix", &matDiscWorld);
				gpApplyShadowShader->SetVector("gObjectColor", &gDiscColor);
				// BeginPass()와 EndPass()사이에서 변수 값을 바꿔줄 때는
				gpApplyShadowShader->CommitChanges();// 를 호출해주어야한다.
				gpDisc->DrawSubset(0);
			}
			gpApplyShadowShader->EndPass();
		}
	}
	gpApplyShadowShader->End();

}

// 디버그 정보 등을 출력.
void RenderInfo()
{
	// 텍스트 색상
	D3DCOLOR fontColor = D3DCOLOR_ARGB(255, 255, 255, 255);

	// 텍스트를 출력할 위치
	RECT rct;
	rct.left = 5;
	rct.right = WIN_WIDTH / 3;
	rct.top = 5;
	rct.bottom = WIN_HEIGHT / 3;

	// 키 입력 정보를 출력
	gpFont->DrawText(NULL, "데모 프레임워크\n\nESC: 데모종료", -1, &rct, 0, fontColor);
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

	// 우선그림자 맵을 만들어준다.
	const int shadowMapSize = 2048;
	// 사이즈를 넣어주고, mipmap 갯수, 용도 플래그를 RENDERTARGET으로 넣어주고, 택스쳐 포멧을 R32F로 지정해준다.
	// 그리고 메모리 클래스, 저장포인터, NULL
	if (FAILED(gpD3DDevice->CreateTexture(shadowMapSize, shadowMapSize,
		1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F,
		D3DPOOL_DEFAULT, &gpShadowRenderTarget, NULL)))
	{
		return false;
	}

	// 깊이 버퍼도 만들어준다.
	// 사이즈, 텍스쳐포맷, 멀티샘플 설정, 멀티샘플 수준, 깊이버퍼를 바꿀 때 내용 보존 안함, 저장포인터, NULL
	if (FAILED(gpD3DDevice->CreateDepthStencilSurface(shadowMapSize, shadowMapSize,
		D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, TRUE,
		&gpShadowDepthStencil, NULL)))
	{
		return false;
	}

	// 모델, 쉐이더, 텍스처등을 로딩
	if (!LoadAssets())
	{
		return false;
	}

	// 폰트를 로딩
	// 1) D3D 디바이스 2) 폰트 높이 3) 폰트 너비 4) 볼드체 5) mipmap 레벨 6) 이텔릭체 여부 7) 문자set 8) 실제 출력폰트가 지정된 속성과 어느정도 비슷해야하는지 설정
	// 9) 실제 출력폰트가 품질이 어느정도 좋아햐 하는지 설정 8) 피치(가로폭)과 패밀리(가족) 설정 9) 폰트 이름 10) 폰트 저장할 out 포인터
	if (FAILED(D3DXCreateFont(gpD3DDevice, 20, 10, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, (DEFAULT_PITCH | FF_DONTCARE),
		"Arial", &gpFont)))
	{
		return false;
	}

	return true;
}

// D3D 객체 및 장치 초기화
bool InitD3D(HWND hWnd)
{
	// D3D 객체
	gpD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!gpD3D)
	{
		return false;
	}

	// D3D장치를 생성하는데 필요한 구조체를 채워넣는다.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.BackBufferWidth = WIN_WIDTH;	// 렌더링 영역, 백버퍼의 너비
	d3dpp.BackBufferHeight = WIN_HEIGHT;	// 백버퍼의 높이
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;// 백버퍼의 포멧
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;// 백버퍼를 스왑할 때의 효과. 성능상 이게 좋다.
	d3dpp.hDeviceWindow = hWnd;
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

bool LoadAssets()
{
	// 텍스처 로딩

	// 쉐이더 로딩
	gpApplyShadowShader = LoadShader("ApplyShadow.fx");
	if (!gpApplyShadowShader)
	{
		return false;
	}
	gpCreateShadowShader = LoadShader("CreateShadow.fx");
	if (!gpCreateShadowShader)
	{
		return false;
	}
	// 모델 로딩
	gpTorus = LoadModel("Torus.x");
	if (!gpTorus)
	{
		return false;
	}
	gpDisc = LoadModel("Disc.x");
	if (!gpDisc)
	{
		return false;
	}
	return true;
}

// 쉐이더 로딩
// .fx 포멧으로 저장된 셰이더 파일
// 정점셰이더와 픽셀셰이더를 모두 포함하고 있는 텍스트 파일이다.
LPD3DXEFFECT LoadShader(const char* filename)
{
	LPD3DXEFFECT ret = NULL;

	LPD3DXBUFFER pError = NULL;
	DWORD dwShaderFlags = 0;

#if _DEBUG
	dwShaderFlags |= D3DXSHADER_DEBUG;
#endif

	// .fx 파일을 로딩 및 컴파일 한다.
	// 1) D3D 디바이스 2. 셰이더 파일 이름 3. 셰이더를 컴파일 할 때 추가로 사용할 전처리#define
	// 4) #include 지시문을 처리할 때 사용할 인터페이스 포인터 5) 셰이더를 컴파일할 때 사용할 플래그
	// 6) 매개변수 공유에 사용할 이펙트 풀 7) 로딩된 이펙트를 저장할 포인터 8) 컴파일 에러 메시지를 가리키는 포인터
	D3DXCreateEffectFromFile(gpD3DDevice, filename,
		NULL, NULL, dwShaderFlags, NULL, &ret, &pError);

	// 쉐이더 로딩에 실패한 경우 output창에 쉐이더
	// 컴파일 에러를 출력한다.
	if (!ret && pError)
	{
		int size = pError->GetBufferSize();
		void* ack = pError->GetBufferPointer();

		if (ack)
		{
			char* str = new char[size];
#pragma warning(suppress : 4996)
			sprintf(str, (const char*)ack, size);
			OutputDebugString(str);
			delete[] str;
		}
	}

	return ret;
}

// 모델 로딩
// .x 포맷으로 저장된 모델을 로딩
// DirectX에서 자체적으로 지원하는 메쉬 포맷이다.
LPD3DXMESH LoadModel(const char* filename)
{
	LPD3DXMESH ret = NULL;
	// 1) 파일명 2) 시스템 메모리에 메쉬를 로딩하기로 설정 3) D3D 장치 4) 인접 삼각형 데이터
	// 5) 메테리얼 정보 6) 이펙트 인스턴스 7) 메테리얼 수 8) 로딩한 메쉬를 저장할 포인터
	if (FAILED(D3DXLoadMeshFromX(filename, D3DXMESH_SYSTEMMEM, gpD3DDevice, NULL, NULL, NULL, NULL, &ret)))
	{
		OutputDebugString("모델 로딩 실패: ");
		OutputDebugString(filename);
		OutputDebugString("\n");
	};

	return ret;
}

// 텍스처 로딩
// 다양한 포맷으로 저장된 이미지들을 텍스처로 로딩
LPDIRECT3DTEXTURE9 LoadTexture(const char* filename)
{
	LPDIRECT3DTEXTURE9 ret = NULL;
	// 1) D3D 장치 2) 파일명 3) 텍스쳐 저장포인터
	if (FAILED(D3DXCreateTextureFromFile(gpD3DDevice, filename, &ret)))
	{
		OutputDebugString("텍스처 로딩 실패: ");
		OutputDebugString(filename);
		OutputDebugString("\n");
	}

	return ret;
}
//------------------------------------------------------------
// 뒷정리 코드.
//------------------------------------------------------------

// 메모리 누수를 방지하기 위해 생성했던 자원들을 모조리 해제 해주어야 한다.
// 개체들도 다 해제해준다.
void Cleanup()
{
	// 폰트를 release 한다.
	if (gpFont)
	{
		gpFont->Release();
		gpFont = NULL;
	}

	// 모델을 release 한다.
	if (gpTorus)
	{
		gpTorus->Release();
		gpTorus = NULL;
	}
	if (gpDisc)
	{
		gpDisc->Release();
		gpDisc = NULL;
	}
	// 쉐이더를 release 한다.
	if (gpCreateShadowShader)
	{
		gpCreateShadowShader->Release();
		gpCreateShadowShader = NULL;
	}
	if (gpApplyShadowShader)
	{
		gpApplyShadowShader->Release();
		gpApplyShadowShader = NULL;
	}
	// 텍스처를 release 한다.
	if (gpShadowDepthStencil)
	{
		gpShadowDepthStencil->Release();
		gpShadowDepthStencil = NULL;
	}
	if (gpShadowRenderTarget)
	{
		gpShadowRenderTarget->Release();
		gpShadowRenderTarget = NULL;
	}
	// D3D를 release 한다.
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

