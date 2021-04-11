#include "d3_chap02.h"

// D3D ����
extern LPDIRECT3D9             gpD3D;				// D3D
extern LPDIRECT3DDEVICE9       gpD3DDevice;				// D3D ��ġ

IDirect3DVertexBuffer9* Cube = NULL;

IDirect3DTexture9* SoyeonTex = NULL;

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

	// ���ؽ� ���۸� �����.
	gpD3DDevice->CreateVertexBuffer(
		36 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&Cube,
		0);

	// ť�� �����ͷ� ���۸� ä���.
	Vertex* vertices;
	Cube->Lock(0, 0, (void**)&vertices, 0);

	// ���� ť���� ���ؽ��� 
	// (�ݽð� �������� �׸����� ����. �޼� ����)
	// (uv ��ǥ��� ������, �Ʒ��� �� ���� �����̴�. 0 ~ 1)
	// ����
	vertices[0] = Vertex(-1.0f, -1.0f, -1.0f,0.0f,-1.0f,0.0f,0.0f,1.0f);
	vertices[1] = Vertex(-1.0f, 1.0f, -1.0f,0.0f,-1.0f,0.0f,0.0f,0.0f);
	vertices[2] = Vertex(1.0f, 1.0f, -1.0f,0.0f,-1.0f,0.0f,1.0f,0.0f);

	vertices[3] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	vertices[4] = Vertex(1.0f, 1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
	vertices[5] = Vertex(1.0f, -1.0f, -1.0f,0.0f,-1.0f,0.0f,1.0f,1.0f);

	// �ĸ�
	vertices[6] = Vertex(1.0f,-1.0f,1.0f,0.0f,1.0f,0.0f,0.0f,1.0f);
	vertices[7] = Vertex(1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	vertices[8] = Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

	vertices[9] = Vertex(1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	vertices[10] = Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	vertices[11] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	// ����
	vertices[12] = Vertex(-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[13] = Vertex(-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertices[14] = Vertex(-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	vertices[15] = Vertex(-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[16] = Vertex(-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	vertices[17] = Vertex(-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// ����
	vertices[18] = Vertex(1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[19] = Vertex(1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertices[20] = Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	vertices[21] = Vertex(1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[22] = Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	vertices[23] = Vertex(1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// ���
	vertices[24] = Vertex(1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	vertices[25] = Vertex(1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	vertices[26] = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

	vertices[27] = Vertex(1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	vertices[28] = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	vertices[29] = Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	// �ϴ�
	vertices[30] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	vertices[31] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
	vertices[32] = Vertex(1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

	vertices[33] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	vertices[34] = Vertex(1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
	vertices[35] = Vertex(1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);

	Cube->Unlock();

	// �� ���
	// ī�޶��� ��ġ�� ������ �����Ѵ�.
	D3DXVECTOR3 pos(0.0f, -2.0f, -3.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);

	gpD3DDevice->SetTransform(D3DTS_VIEW, &V);

	// �ؽ��� �����͸� �ε��Ѵ�.
	D3DXCreateTextureFromFile(
		gpD3DDevice,
		"icon_soyeon.jpg",
		&SoyeonTex);

	// �ؽ��ĸ� Ȱ��ȭ �Ѵ�.
	gpD3DDevice->SetTexture(0, SoyeonTex);

	// �ؽ��� ���͸� �����Ѵ�.
	gpD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	gpD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	gpD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

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

	// gpD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

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

		gpD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		gpD3DDevice->BeginScene();

		gpD3DDevice->SetStreamSource(0, Cube, 0, sizeof(Vertex));
		gpD3DDevice->SetFVF(Vertex::FVF);
		gpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 12);

		gpD3DDevice->EndScene();
		gpD3DDevice->Present(0, 0, 0, 0);
	}
	return true;
}


// D3D ��ü �� ��ġ �ʱ�ȭ
bool InitD3D(HWND hWnd)
{
	// D3D �ʱ�ȭ�� �׻� �̷��� �����Ѵ�.
	gpD3D = Direct3DCreate9(D3D_SDK_VERSION);
	// 1. ��ġ ���� 2. IDirect3DDevice9 ��ü ����
	// + �ùٸ� ��� ���Ϸ� ����� ������ Ȯ��
	if (!gpD3D)
	{
		return false;
	}

	// �⺻ ���÷��� ����� Ư������ D3DCAPS9 ����ü�� ä���.
	D3DCAPS9 caps;
	gpD3D->GetDeviceCaps(
		D3DADAPTER_DEFAULT, // �⺻ ���÷��� ��͸� ǥ���Ѵ�.
		D3DDEVTYPE_HAL,		// ��ġ Ÿ���� �����Ѵ�. 
		&caps				// �⺻ ���÷��� ����� Ư������ ä������.
	);

	int vp = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		// �ϵ���� ���ؽ� ���μ����� �����ȴ�.
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		// ����Ʈ���� ���ؽ� ���μ����� �̿��ؾ� �Ѵ�.
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	// D3Device9�� �ʱ�ȭ�ϴµ� �ʿ��� ����ü�� ä���ִ´�.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.BackBufferWidth = WIN_WIDTH;	// ������ ����, ������� �ʺ� 
	d3dpp.BackBufferHeight = WIN_HEIGHT;	// ������� ���� (�ȼ� �����̴�.)
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;// ������� ���� (�� ��Ʈ ����)
	d3dpp.BackBufferCount = 1;			// ���� �Ѱ��� �ĸ� ���۸� �̿��Ѵ�.
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;	// ��Ƽ ���ø� ����, �ñ��ϸ� ã�ƺ���.
	d3dpp.MultiSampleQuality = 0;		// ��Ƽ ���ø� ����
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;// ����۸� ������ ���� ȿ��. ���ɻ� �̰� ����.
	d3dpp.hDeviceWindow = hWnd;	// 
	d3dpp.Windowed = TRUE;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8; // ����/���ٽ� ������ ����
	d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // ����� �ֻ����� ����۸� �����ϴ� �󵵰��� ����. �̰��� ����Ͱ� ���� ����� ������ ����۸� �������ش�.
	// ���ӿ����� ���ɻ� ������� ���� ���⸦ ��ٸ��� �ʰ� ��ٷ� ������ �ִ� ��찡 ����.(D3DPRESENT IMMEDIATE) ��, �̷� ���� �� �����Ӱ� ���� �������� ���� ���ܺ��̴� ���ۿ��� �ִ�.

	// �� ����(D3DPRESENT_PARAMETERS ����ü)�� ������ D3D��ġ�� �����Ѵ�.
	// 1. ���� ���÷��� ���	2.�̿��� ��ġ Ÿ�� ���� 3. ������ �ڵ�, ���� ��ġ�� ������� ������ ������
	// 4. �ϵ���� Ȥ�� ����Ʈ���� ���μ��� 5. ��ġ Ư���� �Ϻθ� �����ϴ� �ʱ�ȭ�� �ν��Ͻ�
	// 6. ������ ��ġ ����
	if (FAILED(gpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &gpD3DDevice)))
	{
		return false;
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

