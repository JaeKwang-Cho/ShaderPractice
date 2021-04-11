#include "d3_blending.h"

// D3D 관련
extern LPDIRECT3D9             gpD3D;				// D3D
extern LPDIRECT3DDEVICE9       gpD3DDevice;				// D3D 장치



// 모델들
ID3DXMesh* Teapot = 0; // 주전자
D3DMATERIAL9 TeapotMtrl; // 주전자의 제질

IDirect3DVertexBuffer9* BkGndQuad = 0;// 배경 사각형 - 나무상자
IDirect3DTexture9* BkGndTex = 0; // 나무상자 텍스쳐
D3DMATERIAL9	BkGndMtrl; // 배경 재질

struct Vertex
{
	Vertex() {}
	Vertex(
		float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v)
	{
		_x = x;  _y = y;  _z = z;
		_nx = nx; _ny = ny; _nz = nz;
		_u = u;  _v = v;
	}
	float _x, _y, _z;
	float _nx, _ny, _nz;
	float _u, _v;
	static const DWORD FVF;
};
// 정점이 위치, 법선 벡터, 텍스쳐 좌표를 가지고 있다.
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;


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
	// 주전자 메테리얼
	TeapotMtrl = d3light::RED_MTRL;
	TeapotMtrl.Diffuse.a = 0.5f;
	BkGndMtrl = d3light::WHITE_MTRL;
	// 주전자 만들기
	D3DXCreateTeapot(gpD3DDevice, &Teapot, 0);

	// 배경 사각형 만들기
	gpD3DDevice->CreateVertexBuffer(
		6 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		& BkGndQuad,
		0
	);

	Vertex* v;
	BkGndQuad->Lock(0, 0, (void**)&v, 0);

	v[0] = Vertex(-10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-10.0f, 10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = Vertex(10.0f, 10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[3] = Vertex(-10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[4] = Vertex(10.0f, 10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[5] = Vertex(10.0f, -10.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	BkGndQuad->Unlock();

	// 조명 구성하기
	D3DLIGHT9 dir;
	::ZeroMemory(&dir, sizeof(dir));
	dir.Type = D3DLIGHT_DIRECTIONAL;
	dir.Diffuse = d3light::WHITE;
	dir.Specular = d3light::WHITE * 0.2f;
	dir.Ambient = d3light::WHITE * 0.6f;
	dir.Direction = D3DXVECTOR3(0.707f, 0.0f, 0.707f);

	gpD3DDevice->SetLight(0, &dir);
	gpD3DDevice->LightEnable(0, true);
	//정점 법선의 자동 정규화를 유효
	gpD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	gpD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, true);

	// 텍스쳐 필터를 생성하고 세팅함
	D3DXCreateTextureFromFile(
		gpD3DDevice,
		"crate.jpg",
		&BkGndTex
	);
	// D3DTEXTUREFILTERTYPE 의 확대 필터
	gpD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	// D3DTEXTUREFILTERTYPE 의 축소 필터
	gpD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	// 축소시에 사용하는 mipmap 필터
	gpD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	
	// 알파 제질을 난반사로 설정한다.
	// 현재 할당할 수 있는 할당할 수 있는 텍스쳐의 상태값을 설정)
	// 텍스쳐의 최초의 알파인수를 Diffuse로 설정한다는 의미
	gpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	// 텍스쳐와 알파의 혼합처리를 알파인수르 따로 변환하지 않고 사용하겠다는 의미
	gpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// 블렌딩 인수를 지정하여 알파 성분이 투명도를 결정하도록 함
	// 소스의 알파값을 4개 사용하겠다는 의미 (As, As, As, As)
	gpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	// 데스니네이션의 알파 값을 1-(소스 알파) 로 사용한다는 의미 (1-As, 1-As, 1-As, 1-As)
	gpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	
	// 뷰 행렬
	// 카메라의 위치와 방향을 조정한다.
	D3DXVECTOR3 pos(0.0f, 0.0f, -3.0f);
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

		// 갱신
		// 키보드 입력으로 알파를 증가/감소 시킨다.
		if (::GetAsyncKeyState('A') & 0x8000f)
		{
			TeapotMtrl.Diffuse.a += 0.01f;
		}
		if (::GetAsyncKeyState('S') & 0x8000f)
		{
			TeapotMtrl.Diffuse.a -= 0.01f;
		}

		// 알파가 밤위를 벗어나지 않게 한다.
		if (TeapotMtrl.Diffuse.a > 1.0f)
		{
			TeapotMtrl.Diffuse.a = 1.0f;
		}
		if (TeapotMtrl.Diffuse.a < 0.0f)
		{
			TeapotMtrl.Diffuse.a = 0.0f;
		}

		//
		// Draw the scene:
		//

		gpD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		gpD3DDevice->BeginScene();

		// 배경을 그린다.
		D3DXMATRIX W;
		D3DXMatrixIdentity(&W);
		// 월드 행렬변환을 한다.
		gpD3DDevice->SetTransform(D3DTS_WORLD, &W);
		// 정점 스트림 선언 (DWORD)
		gpD3DDevice->SetFVF(Vertex::FVF);
		// 정점 버퍼 정보를 장치 스트림에 연결한다.
		gpD3DDevice->SetStreamSource(0, BkGndQuad, 0, sizeof(Vertex));
		// 제질을 설정한다.
		gpD3DDevice->SetMaterial(&BkGndMtrl);
		// 텍스쳐를 설정한다.
		gpD3DDevice->SetTexture(0, BkGndTex);
		// 도형을 주어진 정보로 그린다.
		gpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		// 주전자를 그린다.
		// 알파 블랜드에 의한 투명화를 활성화 한다.
		gpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

		//1.5배 확대한다.
		D3DXMatrixScaling(&W, 1.5f, 1.5f, 1.5f);
		gpD3DDevice->SetTransform(D3DTS_WORLD, &W);
		gpD3DDevice->SetMaterial(&TeapotMtrl);
		gpD3DDevice->SetTexture(0, 0);
		// 메쉬 서브셋을 그린다.
		Teapot->DrawSubset(0);


		// 다시 알파를 꺼준다.
		gpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

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



