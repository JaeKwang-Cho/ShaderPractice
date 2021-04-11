#include "d3_stencil.h"

// D3D 관련
extern LPDIRECT3D9             gpD3D;				// D3D
extern LPDIRECT3DDEVICE9       gpD3DDevice;				// D3D 장치

IDirect3DVertexBuffer9* VB = NULL;

// 모델관련

IDirect3DTexture9* FloorTex = NULL;
IDirect3DTexture9* WallTex = NULL;
IDirect3DTexture9* MirrorTex = NULL;

D3DMATERIAL9 FloorMtrl = d3d::WHITE_MTRL;
D3DMATERIAL9 WallMtrl = d3d::WHITE_MTRL;
D3DMATERIAL9 MirrorMtrl = d3d::WHITE_MTRL;

ID3DXMesh* Teapot = 0;
D3DXVECTOR3 TeapotPosition(0.0f, 3.0f, -7.5f);
D3DMATERIAL9 TeapotMtrl = d3d::YELLOW_MTRL;


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
	// 벽 반사 약하게
	WallMtrl.Specular = d3d::WHITE * 0.2f;
	// 주전자를 만든다.
	D3DXCreateTeapot(gpD3DDevice, &Teapot, 0);

	//
	//   |----|----|----|
	//   |Wall|Mirr|Wall|
	//   |    | or |    |
	//   /--------------/
	//  /   Floor      /
	// /--------------/
	//
	// 이걸 만든다.

	gpD3DDevice->CreateVertexBuffer(
		24 * sizeof(Vertex),
		0, // usage
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&VB,
		0
	);

	Vertex* v = NULL;
	VB->Lock(0, 0, (void**)&v, 0);

	// 바닥
	v[0] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

	v[3] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[4] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[5] = Vertex(7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	// 왼쪽 벽
	v[6] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[7] = Vertex(-7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[8] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[9] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[10] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[11] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// 오른쪽 벽
	v[12] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[13] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[14] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[15] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[16] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[17] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// 거울
	v[18] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[19] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[20] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[21] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[22] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[23] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	VB->Unlock();

	// 텍스쳐 로딩
	D3DXCreateTextureFromFile(gpD3DDevice, "checker.jpg", &FloorTex);
	D3DXCreateTextureFromFile(gpD3DDevice, "brick0.jpg", &WallTex);
	D3DXCreateTextureFromFile(gpD3DDevice, "ice.bmp", &MirrorTex);

	// 필터 세팅
	gpD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	gpD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	gpD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	// 조명 구성하기
	D3DXVECTOR3 lightDir(0.707f, -0.707f, 0.707f);
	D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f);
	D3DLIGHT9 light = d3d::InitDirectionalLight(&lightDir, &color);

	gpD3DDevice->SetLight(0, &light);
	gpD3DDevice->LightEnable(0, true);

	gpD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	gpD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, true);
	

	
	// 뷰 행렬
	// 카메라의 위치와 방향을 조정한다.
	D3DXVECTOR3 pos(-10.0f, 3.0f, -15.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);

	gpD3DDevice->SetTransform(D3DTS_VIEW, &V);

	// 투영 매트릭스를 지정한다.
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI / 4.0f, // 45 - degree
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
		// 업데이트
		
		static float radius = 20.f;

		// 방향키 입력을 받아서, 주전자 위치를 움직인다.
		if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
			TeapotPosition.x -= 3.0f * timeDelta;

		if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
			TeapotPosition.x += 3.0f * timeDelta;

		// 주전자의 크기를 설정한다..
		if (::GetAsyncKeyState(VK_UP) & 0x8000f)
			radius -= 2.0f * timeDelta;

		if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
			radius += 2.0f * timeDelta;

		static float angle = (3.0f * D3DX_PI) / 2.0f;

		// 키보드 입력을 받아서, 각도를 설정한다..
		if (::GetAsyncKeyState('A') & 0x8000f)
			angle -= 0.5f * timeDelta;

		if (::GetAsyncKeyState('S') & 0x8000f)
			angle += 0.5f * timeDelta;

		// x를 cos, z 좌표를 sin으로 설정
		D3DXVECTOR3 position(cosf(angle) * radius, 3.0f, sinf(angle) * radius);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		// 왼손 좌표계 뷰로 본다. (z가 화면속으로 들어가면 커지는 것)
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		gpD3DDevice->SetTransform(D3DTS_VIEW, &V);
		
		//
		// Draw the scene:
		//

		// 스텐실 버퍼를 초기화 해준다.
		gpD3DDevice->Clear(0,NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER , 0xff000000, 1.0f, 0L);
		gpD3DDevice->BeginScene();

		RenderScene();

		RenderMirror();

		gpD3DDevice->EndScene();
		gpD3DDevice->Present(0, 0, 0, 0);
	}
	return true;
}
void RenderScene()
{
	gpD3DDevice->Clear(0, NULL, D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0L);
	// 주전자 그리기
	gpD3DDevice->SetMaterial(&TeapotMtrl);
	gpD3DDevice->SetTexture(0, 0);
	D3DXMATRIX W;
	D3DXMatrixTranslation(
		&W,
		TeapotPosition.x,
		TeapotPosition.y,
		TeapotPosition.z
	);

	gpD3DDevice->SetTransform(D3DTS_WORLD, &W);
	Teapot->DrawSubset(0);

	// VB로 그릴 준비
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	gpD3DDevice->SetTransform(D3DTS_WORLD, &I);

	gpD3DDevice->SetStreamSource(0, VB, 0, sizeof(Vertex));
	gpD3DDevice->SetFVF(Vertex::FVF);

	// 바닥 그리기
	gpD3DDevice->SetMaterial(&FloorMtrl);
	gpD3DDevice->SetTexture(0, FloorTex);
	gpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	// 벽 그리기
	gpD3DDevice->SetMaterial(&WallMtrl);
	gpD3DDevice->SetTexture(0, WallTex);
	gpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 4);

	// 거울 그리기
	gpD3DDevice->SetMaterial(&MirrorMtrl);
	gpD3DDevice->SetTexture(0, MirrorTex);
	gpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);
	
}
void RenderMirror()
{ 
	gpD3DDevice->Clear(0, NULL, D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0L);

	// 1. 스텐실 버퍼를 활성화 하고 관련된 렌더 상태의 값을 지정한다.
	gpD3DDevice->SetRenderState(D3DRS_STENCILENABLE, true);
	// 항상 스텐실이 성공하도록 함
	gpD3DDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	// 스텐실 테스트 기준치를 1로 설정
	gpD3DDevice->SetRenderState(D3DRS_STENCILREF, 0x1);
	// 마스크도 전부다 넘김
	gpD3DDevice->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	gpD3DDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	// 깊이 테스트가 실패할 경우 스텐실 버퍼를 갱신하지 않음
	gpD3DDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	// 스텐실 테스트가 실패할 경우 갱신 안함 (FUNC - ALWAYS의 경우 필요없음)
	gpD3DDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	// 깊이와 스텐실 테스트가 성공하면 기준치인 1로 설정한다.
	gpD3DDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

	// 2. 스텐실 버퍼에 거울을 렌더링 한다.
	// 일단 스텐실 버퍼에 쓰여야 하므로
	// 깊이 버퍼와 후면 버퍼로 쓰여지는 것을 막는다.
	gpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
	// 알파 블랜드는 true
	gpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	// 원본 블랜드 인수도 0으로 한다.
	gpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
	// 목적지 블랜드 인수를 1으로 지정한다.
	gpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	// 이렇게 하면 후면버퍼가 바뀌지 않는다.

	// 스텐실 버퍼에 거울을 그린다.
	gpD3DDevice->SetStreamSource(0, VB, 0, sizeof(Vertex));
	gpD3DDevice->SetFVF(Vertex::FVF);
	gpD3DDevice->SetMaterial(&MirrorMtrl);
	gpD3DDevice->SetTexture(0, MirrorTex);
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	gpD3DDevice->SetTransform(D3DTS_WORLD, &I);
	// 18번부터 삼각형 2개를 그린다.
	gpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);

	// 깊이버퍼 다시 쓰기를 활성화 시킨다.
	gpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);

	// 이제 스텐셜 버퍼에서 보이는 화면에보이는 픽셀 중 거울에 해당하는 픽셀들은
	// 0x1 값을 가지며 거울로 렌더링 될 부분을 표시한다.

	// 3. 렌더상태를 다시 지정한다. 거울에서만(0x1) 반사되도록 지정 
	gpD3DDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	// 그리고 성공하면 스텐실 버퍼 값을 유지하도록 설정
	gpD3DDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

	//4. 반사된 주전자를 렌더링 하기 위해 계산을 한다.

	// 반사의 위치를 잡는다.
	D3DXMATRIX W, T, R;
	D3DXPLANE plane(0.0f, 0.0f, 1.0f, 0.0f); // xy평면
	D3DXMatrixReflect(&R, &plane); // 지정한 평면에 대해 반전시키는 행렬을 반환

	D3DXMatrixTranslation(
		&T,
		TeapotPosition.x,
		TeapotPosition.y,
		TeapotPosition.z
	); // 주전자의 위치로 이동하는 행렬을 반환

	W = T * R; // 거울에 반사된 주전자의 위치 행렬

	// 5. 반사된 주전자를 렌더링하기 위한 준비

	// 반사된 주전자는 거울보다 뒤에 있으므로 그려지지 않는다.
	// 깊이 버퍼를 일단 없애준다.
	gpD3DDevice->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	// 그리고 거울과 반사된 주전자를 blend 해준다.
	// 목적지가 거울이고 원본이 주전자이므로 다음과 같이 작성해준다.

	// 원본을 목적지 색깔로 블랜딩해주고
	gpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
	// 목적지는 그냥 0으로 한다.
	gpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	// 6. 반사된 이제 주전자를 그린다.

	// 반사된 주전자 월드위치행렬을 적용 시키고
	gpD3DDevice->SetTransform(D3DTS_WORLD, &W);
	gpD3DDevice->SetMaterial(&TeapotMtrl);
	gpD3DDevice->SetTexture(0, 0);

	// 뒷면의 버텍스를 그리지 않고 잘라내는 결정을, 우회전으로 한다.
	gpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	// 물체의 전면과 후면이 뒤바뀌기 때문에, 전면을 결정짓는 CULLing의 방향을 바꾸어 준다.
	Teapot->DrawSubset(0);

	// 다시 블렌딩과 스텐실을 비활성화 하고, culling의 방향을 원래대로 복구한다.
	gpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	gpD3DDevice->SetRenderState(D3DRS_STENCILENABLE, false);
	gpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void Cleanup()
{
	// 전역 변수를 해제 해준다.
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
	d3d::Release<IDirect3DTexture9*>(FloorTex);
	d3d::Release<IDirect3DTexture9*>(WallTex);
	d3d::Release<IDirect3DTexture9*>(MirrorTex);
	d3d::Release<ID3DXMesh*>(Teapot);
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



