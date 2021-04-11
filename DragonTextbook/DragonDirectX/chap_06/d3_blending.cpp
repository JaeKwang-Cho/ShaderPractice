#include "d3_blending.h"

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
	// ������ ���׸���
	TeapotMtrl = d3light::RED_MTRL;
	TeapotMtrl.Diffuse.a = 0.5f;
	BkGndMtrl = d3light::WHITE_MTRL;
	// ������ �����
	D3DXCreateTeapot(gpD3DDevice, &Teapot, 0);

	// ��� �簢�� �����
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

	// �ؽ��� ���͸� �����ϰ� ������
	D3DXCreateTextureFromFile(
		gpD3DDevice,
		"crate.jpg",
		&BkGndTex
	);
	// D3DTEXTUREFILTERTYPE �� Ȯ�� ����
	gpD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	// D3DTEXTUREFILTERTYPE �� ��� ����
	gpD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	// ��ҽÿ� ����ϴ� mipmap ����
	gpD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	
	// ���� ������ ���ݻ�� �����Ѵ�.
	// ���� �Ҵ��� �� �ִ� �Ҵ��� �� �ִ� �ؽ����� ���°��� ����)
	// �ؽ����� ������ �����μ��� Diffuse�� �����Ѵٴ� �ǹ�
	gpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	// �ؽ��Ŀ� ������ ȥ��ó���� �����μ��� ���� ��ȯ���� �ʰ� ����ϰڴٴ� �ǹ�
	gpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// ���� �μ��� �����Ͽ� ���� ������ ������ �����ϵ��� ��
	// �ҽ��� ���İ��� 4�� ����ϰڴٴ� �ǹ� (As, As, As, As)
	gpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	// �����ϳ��̼��� ���� ���� 1-(�ҽ� ����) �� ����Ѵٴ� �ǹ� (1-As, 1-As, 1-As, 1-As)
	gpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	
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

		// ����
		// Ű���� �Է����� ���ĸ� ����/���� ��Ų��.
		if (::GetAsyncKeyState('A') & 0x8000f)
		{
			TeapotMtrl.Diffuse.a += 0.01f;
		}
		if (::GetAsyncKeyState('S') & 0x8000f)
		{
			TeapotMtrl.Diffuse.a -= 0.01f;
		}

		// ���İ� ������ ����� �ʰ� �Ѵ�.
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

		// ����� �׸���.
		D3DXMATRIX W;
		D3DXMatrixIdentity(&W);
		// ���� ��ĺ�ȯ�� �Ѵ�.
		gpD3DDevice->SetTransform(D3DTS_WORLD, &W);
		// ���� ��Ʈ�� ���� (DWORD)
		gpD3DDevice->SetFVF(Vertex::FVF);
		// ���� ���� ������ ��ġ ��Ʈ���� �����Ѵ�.
		gpD3DDevice->SetStreamSource(0, BkGndQuad, 0, sizeof(Vertex));
		// ������ �����Ѵ�.
		gpD3DDevice->SetMaterial(&BkGndMtrl);
		// �ؽ��ĸ� �����Ѵ�.
		gpD3DDevice->SetTexture(0, BkGndTex);
		// ������ �־��� ������ �׸���.
		gpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		// �����ڸ� �׸���.
		// ���� ���忡 ���� ����ȭ�� Ȱ��ȭ �Ѵ�.
		gpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

		//1.5�� Ȯ���Ѵ�.
		D3DXMatrixScaling(&W, 1.5f, 1.5f, 1.5f);
		gpD3DDevice->SetTransform(D3DTS_WORLD, &W);
		gpD3DDevice->SetMaterial(&TeapotMtrl);
		gpD3DDevice->SetTexture(0, 0);
		// �޽� ������� �׸���.
		Teapot->DrawSubset(0);


		// �ٽ� ���ĸ� ���ش�.
		gpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

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



