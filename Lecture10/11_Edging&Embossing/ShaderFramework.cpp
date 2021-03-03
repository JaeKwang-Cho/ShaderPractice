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
LPDIRECT3D9             gpD3D				= NULL;				// D3D
LPDIRECT3DDEVICE9       gpD3DDevice			= NULL;			// D3D 장치

// 폰트
ID3DXFont* gpFont = NULL;

// 모델
LPD3DXMESH				gpTeapot			= NULL;
// 쉐이더
LPD3DXEFFECT			gpEnvironmentMappingShader = NULL;
LPD3DXEFFECT			gpNoEffect	= NULL;
LPD3DXEFFECT			gpGrayScale = NULL;
LPD3DXEFFECT			gpSepia = NULL;
LPD3DXEFFECT			gpEdgeDetection = NULL;
LPD3DXEFFECT			gpEmboss = NULL;


// 텍스처
LPDIRECT3DTEXTURE9		gpStoneDM			= NULL;
LPDIRECT3DTEXTURE9		gpStoneSM			= NULL;
LPDIRECT3DTEXTURE9		gpStoneNM			= NULL;
LPDIRECT3DCUBETEXTURE9	gpSnowENV			= NULL;

// 장면 렌더타깃
LPDIRECT3DTEXTURE9		gpSceneRenderTarget = NULL;

// 화면을 가득 채우는 사각형
LPDIRECT3DVERTEXDECLARATION9	gpFullscreenQuadDecl = NULL; // 정점 쉐이더로 입력되는 정점의 데이터 정보
LPDIRECT3DVERTEXBUFFER9			gpFullscreenQuadVB = NULL; // 정점 버퍼 리소스를 조작하는 인터페이스
LPDIRECT3DINDEXBUFFER9			gpFullscreenQuadIB = NULL; // 인덱스 버퍼 리소스를 조작하는 인터페이스

// 빛의 색상
D3DXVECTOR4				gLightColor(0.7f, 0.7f, 1.0f, 1.0f);

// 프로그램 이름
const char* gAppName = "Picturing 쉐이더 프레임워크";
// 회전값
float gRotationY = 0.0f;
// 사용할 포스트프로세스 셰이더의 색인
int		gPostProcessIndex = 0;
// 빛의 위치
D3DXVECTOR4				gWorldLightPosition(500.0f, 500.0f, -500.0f, 1.0f);
// 카메라 위치
D3DXVECTOR4				gWorldCameraPosition(0.0f, 0.0f, -200.0f, 1.0f);


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
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
		gPostProcessIndex = keyPress - '0' - 1;
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
	D3DCOLOR bgColour = 0x00000000;	// 배경색상 - 검정

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
	/////////////
	// 1. 장면을 렌더타깃 안에 그린다.
	/////////////

	// 현재 하드웨어 백버퍼
	LPDIRECT3DSURFACE9 pHWBackBuffer = NULL;
	gpD3DDevice->GetRenderTarget(0, &pHWBackBuffer);

	// 렌더타깃 위에 그린다.
	LPDIRECT3DSURFACE9 pSceneSurface = NULL;
	if (SUCCEEDED(gpSceneRenderTarget->GetSurfaceLevel(0, &pSceneSurface)))
	{
		gpD3DDevice->SetRenderTarget(0, pSceneSurface);
		pSceneSurface->Release();
		pSceneSurface = NULL;
	}
	// 깊이 버퍼를 따로 사용하지 않는 이유는 렌더타깃의 크기가 하드웨어 깊이 버퍼의 크기와 동일하기 때문

	// 저번 프레임에 그렸던 장면을 지운다.
	gpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0xFF000000, 1.0f, 0);
	// 이렇게 하면 모든 렌더링 결과가 렌더타깃 안에 들어간다.

	// 뷰 행렬을 만든다.
	D3DXMATRIXA16 matView;
	D3DXVECTOR3 vEyePt(gWorldCameraPosition.x, gWorldCameraPosition.y, gWorldCameraPosition.z);	// 카메라의 위치 벡터
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);	// 바라보는 곳의 위치 벡터
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);		// 카메라 위를 가리키는 벡터

	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);

	// 투영 행렬을 만든다. (원근 투시법)
	D3DXMATRIXA16 matProjection;
	// 위에서 정의했던 값들을 넣어준다.
	D3DXMatrixPerspectiveFovLH(&matProjection, FOV, ASPECT_RATIO, NEAR_PLANE, FAR_PLANE);
	
	// 지구를 y축 중심으로 조금씩 회전 시킨다
	gRotationY += 0.4f * PI / 180.0f;
	if (gRotationY > 2 * PI)
	{
		gRotationY -= 2 * PI;
	}

	// 월드 행렬을 만든다.
	// 월드 행렬은 1. 한 물체의 위치와 방위 2.확장/축소 변환을 합친 것이다.
	// *** 뷰행렬 및 투영행렬과 달리 각 물체마다 월드행렬을 만들어 주어야한다.
	D3DXMATRIXA16 matWorld;
	D3DXMatrixRotationY(&matWorld,gRotationY); // y축으로 돌리는 월드 행렬을 만든다.

	// 월드/뷰/투영행렬을 미리 곱한다
	D3DXMATRIXA16	matWorldView;
	D3DXMATRIXA16	matWorldViewProjection;
	D3DXMatrixMultiply(&matWorldView, &matWorld, &matView);
	D3DXMatrixMultiply(&matWorldViewProjection, &matWorldView, &matProjection);

	gpEnvironmentMappingShader->SetMatrix("gWorldMatrix", &matWorld);
	gpEnvironmentMappingShader->SetMatrix("gWorldViewProjectionMatrix", &matWorldViewProjection);

	gpEnvironmentMappingShader->SetVector("gWorldLightPosition", &gWorldLightPosition);
	gpEnvironmentMappingShader->SetVector("gWorldCameraPosition", &gWorldCameraPosition);

	gpEnvironmentMappingShader->SetVector("gLightColor", &gLightColor);
	gpEnvironmentMappingShader->SetTexture("DiffuseMap_Tex", gpStoneDM);
	gpEnvironmentMappingShader->SetTexture("SpecularMap_Tex", gpStoneSM);
	gpEnvironmentMappingShader->SetTexture("DiffuseMap_Tex", gpStoneNM);
	gpEnvironmentMappingShader->SetTexture("EnvironmentMap_Tex",gpSnowENV);



	// 이제 셰이더가 가진 값들을 그릴 물체들에 적용하라고 GPU에게 명령한다.
	// 모양과 함수들을 잘 보자
	UINT numPasses = 0;
	gpEnvironmentMappingShader->Begin(&numPasses, NULL); // Begin()과 End() 사이에
	{
		for (UINT i = 0; i < numPasses; ++i)
		{
			gpEnvironmentMappingShader->BeginPass(i); // BeginPass()와 EndPass() 가 있고
			{
				// 구체를 그린다.
				gpTeapot->DrawSubset(0); // 그 사이에 .x를 그리는 DrawSubset()이 있다.
			}
			gpEnvironmentMappingShader->EndPass();
		}
	}
	gpEnvironmentMappingShader->End();
	// 이렇게 하면 GPU가 gpColorShader를 이용해서 gpSphere를 그린다.

	///////////////
	//2. 포스트 프로세싱을 적용한다.
	///////////////

	// 하드웨어 백버퍼를 다시 사용한다.
	gpD3DDevice->SetRenderTarget(0, pHWBackBuffer);
	pHWBackBuffer->Release();
	pHWBackBuffer = NULL;

	// 사용할 포스트프세스 효과
	LPD3DXEFFECT effectToUse = gpNoEffect;
	if (gPostProcessIndex == 1)
	{
		effectToUse = gpGrayScale;
	}
	else if (gPostProcessIndex == 2)
	{
		effectToUse = gpSepia;
	}
	else if (gPostProcessIndex == 3)
	{
		effectToUse = gpEdgeDetection;
	}
	else if (gPostProcessIndex == 4)
	{
		effectToUse = gpEmboss;
	}

	D3DXVECTOR4 pixelOffset(1 / (float)WIN_WIDTH, 1 / (float)WIN_HEIGHT, 0, 0);
	if (effectToUse == gpEdgeDetection || effectToUse == gpEmboss)
	{
		effectToUse->SetVector("gPixelOffset", &pixelOffset);
	}

	// 텍스쳐를 설정하고
	// pass를 시작한다.
	effectToUse->SetTexture("SceneTexture_Tex", gpSceneRenderTarget);
	effectToUse->Begin(&numPasses, NULL);
	{
		for (UINT i = 0; i < numPasses; ++i)
		{
			// 화면 가득 사각형을 그린다.
			effectToUse->BeginPass(i);
			{
				// 우리가 그릴 사각형은 D3DXMESH가 아니여서
				// DrawSubet() 함수가 하는일을 직접 해주어야 한다.
				// 1) 정점 버퍼를 D3D에 세팅한다.
				// 2) 정점 버퍼 선언을 D3D에 설정하여 정점 포맷을 알려준다.
				// 3) 인덱스 버퍼를 D3D 장치에 설정한다.
				// 4) 그리기 함수를 호출한다.

				gpD3DDevice->SetStreamSource(0, gpFullscreenQuadVB, 0, sizeof(float) * 5);
				// 인덱스 버퍼와 정점선언을 설정
				gpD3DDevice->SetIndices(gpFullscreenQuadIB);
				gpD3DDevice->SetVertexDeclaration(gpFullscreenQuadDecl);

				// 마지막으로 그리기 함수
				// 1) 색인 데이터를 삼각형리스트로 처리하는 이넘
				// 2) 첫번째 정점주터 그린다.
				// 3) 최소 색인은 0
				// 4) 총 6개의 정점
				// 5) 첫번째 색인 부터 사용
				// 6) 총 2개의 삼각형을 그린다.
				gpD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 6, 0, 2);
			}
			effectToUse->EndPass();
		}
	}
	effectToUse->End();
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
	gpFont->DrawText(NULL, "데모 프레임워크\n\nESC: 데모종료\n1: 칼라\n2: 흑백\n3: 세피아\n4: 외곽선\n5: 양각효과", -1, &rct, 0, fontColor);
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
	// 화면을 가득 채우는 사각형을 하나 만든다.
	InitFullScreenQuad();
	
	// 렌더타깃을 만든다.
	if (FAILED(gpD3DDevice->CreateTexture(WIN_WIDTH, WIN_HEIGHT,
		1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8,
		D3DPOOL_DEFAULT, &gpSceneRenderTarget, NULL)))
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
	gpStoneDM = LoadTexture("fieldstone_DM.tga");
	if (!gpStoneDM)
	{
		return false;
	}

	gpStoneSM = LoadTexture("fieldstone_SM.tga");
	if (!gpStoneSM)
	{
		return false;
	}

	gpStoneNM = LoadTexture("fieldstone_NM.tga");
	if (!gpStoneNM)
	{
		return false;
	}

	D3DXCreateCubeTextureFromFile(gpD3DDevice, "Snow_ENV.dds", &gpSnowENV);
	if (!gpSnowENV)
	{
		return false;
	}

	// 쉐이더 로딩
	gpEnvironmentMappingShader = LoadShader("EnvironmentMapping.fx");
	if (!gpEnvironmentMappingShader)
	{
		return false;
	}

	gpNoEffect = LoadShader("NoEffect.fx");
	if (!gpNoEffect)
	{
		return false;
	}

	gpGrayScale = LoadShader("Grayscale.fx");
	if (!gpGrayScale)
	{
		return false;
	}

	gpSepia = LoadShader("Sepia.fx");
	if (!gpSepia)
	{
		return false;
	}

	gpEdgeDetection = LoadShader("EdgeDetection.fx");
	if (!gpEdgeDetection)
	{
		return false;
	}

	gpEmboss = LoadShader("Emboss.fx");
	if (!gpEmboss)
	{
		return false;
	}

	// 모델 로딩
	gpTeapot = LoadModel("TeapotWithTangent.x");
	if (!gpTeapot)
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
	if (gpTeapot)
	{
		gpTeapot->Release();
		gpTeapot = NULL;
	}

	// 쉐이더를 release 한다.
	if (gpEnvironmentMappingShader)
	{
		gpEnvironmentMappingShader->Release();
		gpEnvironmentMappingShader = NULL;
	}

	if (gpNoEffect)
	{
		gpNoEffect->Release();
		gpNoEffect = NULL;
	}

	if (gpGrayScale)
	{
		gpGrayScale->Release();
		gpGrayScale = NULL;
	}

	if (gpSepia)
	{
		gpSepia->Release();
		gpSepia = NULL;
	}
	if (gpEdgeDetection)
	{
		gpEdgeDetection->Release();
		gpEdgeDetection = NULL;
	}
	if (gpEmboss)
	{
		gpEmboss->Release();
		gpEmboss = NULL;
	}

	// 텍스처를 release 한다.
	if (gpStoneDM)
	{
		gpStoneDM->Release();
		gpStoneDM = NULL;
	}

	if (gpStoneSM)
	{
		gpStoneSM->Release();
		gpStoneSM = NULL;
	}

	if (gpStoneNM)
	{
		gpStoneNM->Release();
		gpStoneNM = NULL;
	}

	if (gpSnowENV)
	{
		gpSnowENV->Release();
		gpSnowENV = NULL;
	}

	// 화면크기 사각형을 해제한다
	if (gpFullscreenQuadDecl)
	{
		gpFullscreenQuadDecl->Release();
		gpFullscreenQuadDecl = NULL;
	}

	if (gpFullscreenQuadVB)
	{
		gpFullscreenQuadVB->Release();
		gpFullscreenQuadVB = NULL;
	}

	if (gpFullscreenQuadIB)
	{
		gpFullscreenQuadIB->Release();
		gpFullscreenQuadIB = NULL;
	}

	//렌더타깃을 해제한다
	if (gpSceneRenderTarget)
	{
		gpSceneRenderTarget->Release();
		gpSceneRenderTarget = NULL;
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

void InitFullScreenQuad()
{
	// 정점 선언을 만든다.
	D3DVERTEXELEMENT9 vtxDesc[3]; // 정점 정보를 가진 구조체

	// 첫번째 요소인 위치를 선언
	int offset = 0;
	int i = 0;

	// 위치
	vtxDesc[i].Stream = 0; // D3D 장치에 정점 버퍼들이 동시에 여러개 넣을 수 있는데, 그때 슬롯의 인덱스이다.
	vtxDesc[i].Offset = offset; // 정점정보가 시작하는 메모리 위치에서 현재 요소가 시작하는 곳까지의 오프셋 값입니다. 
	vtxDesc[i].Type = D3DDECLTYPE_FLOAT3; // 데이터 형입니다. 정점 정보이기에 float3
	vtxDesc[i].Method = D3DDECLMETHOD_DEFAULT; // 궁금하면 찾아보자. 보통 이거쓴다.
	vtxDesc[i].Usage = D3DDECLUSAGE_POSITION; // 용도를 지정한다.
	vtxDesc[i].UsageIndex = 0; // 시멘틱 인덱스 (그 뒤에 붙였던 숫자를 말한다.)

	// 두번째 요소인 UV 좌표를 선언
	offset += sizeof(float) * 3;
	++i;

	// UV 좌표 0
	vtxDesc[i].Stream = 0;
	vtxDesc[i].Offset = offset;
	vtxDesc[i].Type = D3DDECLTYPE_FLOAT2; // UV 좌표이므로 float2
	vtxDesc[i].Method = D3DDECLMETHOD_DEFAULT;
	vtxDesc[i].Usage = D3DDECLUSAGE_TEXCOORD; // UV 좌표이므로 TEXCOORD
	vtxDesc[i].UsageIndex = 0;

	// 모든 정점 요소들을 다 더했다는 표시
	offset += sizeof(float) * 2;
	++i;

	// 정점포맷의 끝임을 표현 (D3DDECL_END())
	vtxDesc[i].Stream = 0xFF;
	vtxDesc[i].Offset = 0;
	vtxDesc[i].Type = D3DDECLTYPE_UNUSED;
	vtxDesc[i].Method = 0;
	vtxDesc[i].Usage = 0;
	vtxDesc[i].UsageIndex = 0;

	//이제 이 배열을 가지고, D3D 장치에 정점 선언을 만들자
	gpD3DDevice->CreateVertexDeclaration(vtxDesc, &gpFullscreenQuadDecl);
	// 만든 정점 선언은, 나중에 mesh를 그릴때 D3D에게 정점버퍼의 포맷에 맞춰서 올바른 데이터를 뽑아가도록 하는데에 쓰인다.

	// 정점 버퍼를 만든다.
	// 1) 정점 버퍼의 크기, 위에서 한 정점에 들어가는 바이트 수가 있고, 4각 형을 만들 것이기 때문에 4를 곱해준다.
	// 2) 특별한 용도로 사용하지 않는다.
	// 3) FVF(flexible vertex format)를 설정하는 곳이다.
	// 4) D3D가 알아서 관리해주는 메모리 풀을 사용한다.
	// 5) 저장용 포인터
	// 6) 언제나 NULL
	gpD3DDevice->CreateVertexBuffer(offset * 4, 0, 0, D3DPOOL_MANAGED, &gpFullscreenQuadVB, NULL);

	// 정점 버퍼에 내용을 채워 넣는다.
	void* vertexData = NULL;
	// 인덱스 데이터의 범위를 잠그어, 인덱스 버퍼 메모리의 포인터를 얻어온다.
	gpFullscreenQuadVB->Lock(0, 0, &vertexData, 0);
	{
		// 메모리에 정점 정보를 차례대로 써넣는다.
		float* data = (float*)vertexData;
		*data++ = -1.0f;	*data++ = 1.0f;		*data++ = 0.0f; // 정점
		*data++ = 0.0f;		*data++ = 0.0f;						// UV

		*data++ = 1.0f;		*data++ = 1.0f;		*data++ = 0.0f;
		*data++ = 1.0f;		*data++ = 0;

		*data++ = 1.0f;		*data++ = -1.0f;	*data++ = 0.0f;
		*data++ = 1.0f;		*data++ = 1.0f;

		*data++ = -1.0f;	*data++ = -1.0f;	*data++ = 0.0f;
		*data++ = 0.0f;		*data++ = 1.0f;
	}
	gpFullscreenQuadVB->Unlock(); // 락을 풀어준다

	// 인덱스 버퍼를 만들어준다
	// 1) 색인 버퍼의 크기 일단 16비트 6개를 쓰겠다.
	// 2) 특별한 용도로 사용하지 않는다.
	// 3) 각 색인이 16비트임을 명시
	// 4) D3D가 알아서 관리해주는 메모리 풀 사용
	// 5) 저장포인터
	// 6) 항상 널
	gpD3DDevice->CreateIndexBuffer(sizeof(short) * 6, 0, D3DFMT_INDEX16,
		D3DPOOL_MANAGED, &gpFullscreenQuadIB, NULL);

	// 비슷한 방법으로 인덱스 버퍼에 데이터를 넣자.
	void* indexData = NULL;
	gpFullscreenQuadIB->Lock(0, 0, &indexData, 0);
	{
		unsigned short* data = (unsigned short*)indexData;
		// 시계방향으로 삼각형을 2개 만들어준다.
		*data++ = 0;	*data++ = 1;	*data++ = 3;
		*data++ = 3;	*data++ = 1;	*data++ = 2;
	}
	gpFullscreenQuadIB->Unlock();

	// 이러면 끝이다.
}
