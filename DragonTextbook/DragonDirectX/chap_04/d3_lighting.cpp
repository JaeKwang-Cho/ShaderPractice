#include "d3_lighting.h"

// D3D 관련
extern LPDIRECT3D9             gpD3D;				// D3D
extern LPDIRECT3DDEVICE9       gpD3DDevice;				// D3D 장치

// 법선 벡터추가
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL;

// 피라미드 버퍼 추가
IDirect3DVertexBuffer9* Pyramid = NULL;


D3DMATERIAL9 d3light::InitMtrl(D3DXCOLOR ambient, D3DXCOLOR diffuse, D3DXCOLOR specular, D3DXCOLOR emissive, float power)
{
	D3DMATERIAL9 mtrl;

	mtrl.Ambient = ambient;
	mtrl.Diffuse = diffuse;
	mtrl.Specular = specular;
	mtrl.Emissive = emissive;
	mtrl.Power = power;

	return mtrl;
}
D3DLIGHT9 d3light::InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color)
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
D3DLIGHT9 d3light::InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color)
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
D3DLIGHT9 d3light::InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color)
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
bool Setup()
{
	gpD3DDevice->SetRenderState(D3DRS_LIGHTING, true);

	gpD3DDevice->CreateVertexBuffer(
		12 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&Pyramid,
		NULL
	);

	// 피라미드 데이터로 버텍스 버퍼를 채운다.
	Vertex* v;
	Pyramid->Lock(0, 0, (void**)&v, 0);

	// 정면
	v[0] = Vertex(-1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f);
	v[1] = Vertex(0.0f, 1.0f, 0.0f, 0.0f, 0.707f, -0.707f);
	v[2] = Vertex(1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f);

	// 왼쪽 측면
	v[3] = Vertex(-1.0f, 0.0f, 1.0f, -0.707f, 0.707f, 0.0f);
	v[4] = Vertex(0.0f, 1.0f, 0.0f, -0.707f, 0.707f, 0.0f);
	v[5] = Vertex(-1.0f, 0.0f, -1.0f, -0.707f, 0.707f, 0.0f);

	// 오른쪽 측면
	v[6] = Vertex(1.0f, 0.0f, -1.0f, 0.707f, 0.707f, 0.0f);
	v[7] = Vertex(0.0f, 1.0f, 0.0f, 0.707f, 0.707f, 0.0f);
	v[8] = Vertex(1.0f, 0.0f, 1.0f, 0.707f, 0.707f, 0.0f);

	// 후면
	v[9] = Vertex(1.0f, 0.0f, 1.0f, 0.0f, 0.707f, 0.707f);
	v[10] = Vertex(0.0f, 1.0f, 0.0f, 0.0f, 0.707f, 0.707f);
	v[11] = Vertex(-1.0f, 0.0f, 1.0f, 0.0f, 0.707f, 0.707f);



	Pyramid->Unlock();

	// 재질
	D3DMATERIAL9 mtrl;
	mtrl.Ambient = d3light::WHITE;
	mtrl.Diffuse = d3light::RED;
	mtrl.Specular = d3light::WHITE;
	mtrl.Emissive = d3light::BLACK;
	mtrl.Power = 5.0f;

	gpD3DDevice->SetMaterial(&mtrl);

	// 광원 설정
	D3DLIGHT9 dir;
	::ZeroMemory(&dir, sizeof(dir));
	dir.Type = D3DLIGHT_DIRECTIONAL;
	dir.Diffuse = d3light::WHITE;
	dir.Specular = d3light::WHITE * 0.3f;
	dir.Ambient = d3light::WHITE * 0.6f;
	dir.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	gpD3DDevice->SetLight(0, &dir);
	gpD3DDevice->LightEnable(0, true);

	// 법선 정리
	gpD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	gpD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, true);

	// 뷰 행렬
	// 카메라의 위치와 방향을 조정한다.
	D3DXVECTOR3 pos(0.0f, 1.0f, -3.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);

	gpD3DDevice->SetTransform(D3DTS_VIEW, &V);

	// 투영 매트릭스를 지정한다.
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f, // 90 - degree
		(float)WIN_WIDTH / (float)WIN_HEIGHT,
		1.0f,
		1000.0f);

	gpD3DDevice->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

bool Display(float timeDelta)
{
	if (gpD3DDevice)
	{
		D3DXMATRIX yRot;

		static float y = 0.0f;

		D3DXMatrixRotationY(&yRot, y);
		y += timeDelta;

		if (y >= 6.28f)
			y = 0.0f;

		gpD3DDevice->SetTransform(D3DTS_WORLD, &yRot);

		//
		// Draw the scene:
		//

		gpD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		gpD3DDevice->BeginScene();

		gpD3DDevice->SetStreamSource(0, Pyramid, 0, sizeof(Vertex));
		gpD3DDevice->SetFVF(Vertex::FVF);
		gpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 4);

		gpD3DDevice->EndScene();
		gpD3DDevice->Present(0, 0, 0, 0);
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


