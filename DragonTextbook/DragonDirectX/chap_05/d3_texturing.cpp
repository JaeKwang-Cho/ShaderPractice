#include "d3_texturing.h"

// D3D ����
extern LPDIRECT3D9             gpD3D;				// D3D
extern LPDIRECT3DDEVICE9       gpD3DDevice;				// D3D ��ġ

// �ؽ��� ��ǥ
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL| D3DFVF_TEX1;

IDirect3DVertexBuffer9* Quad = 0;
IDirect3DTexture9* Tex = 0;



//------------------------------------------------------------
// �ʱ�ȭ �ڵ�
//------------------------------------------------------------
bool InitEverything(HWND hWnd)
{
	// D3D�� �ʱ�ȭ
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
	gpD3DDevice->CreateVertexBuffer(
		6 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&Quad, NULL
	);

	Vertex* v;
	Quad->Lock(0, 0, (void**)&v, 0);

	// �ΰ��� �ﰢ������ ������ �簢��, �ؽ��� ��ǥ�� Ȯ������.

	v[0] = Vertex(-1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-1.0f, 1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = Vertex(1.0f, 1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[3] = Vertex(-1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[4] = Vertex(1.0f, 1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[5] = Vertex(1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	Quad->Unlock();

	// �ؽ��� �����͸� �ε��Ѵ�.
	D3DXCreateTextureFromFile(
		gpD3DDevice,
		"soyeon.bmp",
		&Tex);

	// �ؽ��ĸ� Ȱ��ȭ �Ѵ�.
	gpD3DDevice->SetTexture(0, Tex);

	// �ؽ��� ���͸� �����Ѵ�.
	gpD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	gpD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	gpD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	// ���⼭�� ������ �Ⱦ���.
	gpD3DDevice->SetRenderState(D3DRS_LIGHTING, false);

	// ���� ��Ʈ������ �����Ѵ�.
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
		

		//
		// Draw the scene:
		//

		gpD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		gpD3DDevice->BeginScene();

		gpD3DDevice->SetStreamSource(0, Quad, 0, sizeof(Vertex));
		gpD3DDevice->SetFVF(Vertex::FVF);
		gpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		gpD3DDevice->EndScene();
		gpD3DDevice->Present(0, 0, 0, 0);
	}
	return true;
}


void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(Quad);
	d3d::Release<IDirect3DTexture9*>(Tex);
	// ���������� D3D�� release �Ѵ�.
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


