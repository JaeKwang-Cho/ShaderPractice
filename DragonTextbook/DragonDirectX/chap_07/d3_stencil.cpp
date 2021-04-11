#include "d3_stencil.h"

// D3D ����
extern LPDIRECT3D9             gpD3D;				// D3D
extern LPDIRECT3DDEVICE9       gpD3DDevice;				// D3D ��ġ



// �𵨵�
ID3DXMesh* Teapot = 0; // ������
D3DMATERIAL9 TeapotMtrl; // �������� ����

IDirect3DVertexBuffer9* BkGndQuad = 0;// ��� �簢�� - ��������
IDirect3DTexture9* BkGndTex = 0; // �������� �ؽ���
D3DMATERIAL9	BkGndMtrl; // ��� ����

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
// ������ ��ġ, ���� ����, �ؽ��� ��ǥ�� ������ �ִ�.
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;


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
	

	// ���� �����ϱ�
	D3DLIGHT9 dir;
	::ZeroMemory(&dir, sizeof(dir));
	dir.Type = D3DLIGHT_DIRECTIONAL;
	dir.Diffuse = d3light::WHITE;
	dir.Specular = d3light::WHITE * 0.2f;
	dir.Ambient = d3light::WHITE * 0.6f;
	dir.Direction = D3DXVECTOR3(0.707f, 0.0f, 0.707f);

	gpD3DDevice->SetLight(0, &dir);
	gpD3DDevice->LightEnable(0, true);
	//���� ������ �ڵ� ����ȭ�� ��ȿ
	gpD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	gpD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, true);

	
	// �� ���
	// ī�޶��� ��ġ�� ������ �����Ѵ�.
	D3DXVECTOR3 pos(0.0f, 0.0f, -3.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);

	gpD3DDevice->SetTransform(D3DTS_VIEW, &V);

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
		D3DXMATRIX yRot;

		static float y = 0.0f;

		D3DXMatrixRotationY(&yRot, y);
		y += timeDelta;

		if (y >= 6.28f)
			y = 0.0f;

		
		//
		// Draw the scene:
		//

		gpD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		gpD3DDevice->BeginScene();

		

		gpD3DDevice->EndScene();
		gpD3DDevice->Present(0, 0, 0, 0);
	}
	return true;
}


void Cleanup()
{
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



