#include "d3_draw.h"

extern LPDIRECT3D9             gpD3D;				// D3D
extern LPDIRECT3DDEVICE9       gpD3DDevice;			// D3D ��ġ

extern IDirect3DVertexBuffer9* VB;
extern IDirect3DIndexBuffer9* IB;

const DWORD Vertex::FVF = D3DFVF_XYZ;


//------------------------------------------------------------
// �ʱ�ȭ �ڵ�
//------------------------------------------------------------
bool InitEverything(HWND hWnd)
{

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
	// ���ؽ��� �ε��� ���۸� �����.
	gpD3DDevice->CreateVertexBuffer(
		8 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&VB,
		0);

	gpD3DDevice->CreateIndexBuffer(
		36 * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&IB,
		0);

	// ť�� �����ͷ� ���۸� ä���.
	Vertex* vertices;
	VB->Lock(0, 0, (void**)&vertices, 0);

	// ���� ť���� ���ؽ���
	vertices[0] = Vertex(-1.0f, -1.0f, -1.0f);
	vertices[1] = Vertex(-1.0f, 1.0f, -1.0f);
	vertices[2] = Vertex(1.0f, 1.0f, -1.0f);
	vertices[3] = Vertex(1.0f, -1.0f, -1.0f);
	vertices[4] = Vertex(-1.0f, -1.0f, 1.0f);
	vertices[5] = Vertex(-1.0f, 1.0f, 1.0f);
	vertices[6] = Vertex(1.0f, 1.0f, 1.0f);
	vertices[7] = Vertex(1.0f, -1.0f, 1.0f);

	VB->Unlock();

	// �Թ�ü�� �ﰢ������ �����Ѵ�.
	WORD* indices;
	IB->Lock(0, 0, (void**)&indices, 0);

	// ����
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 0; indices[4] = 2; indices[5] = 3;

	// �ĸ�
	indices[6] = 4; indices[7] = 6; indices[8] = 5;
	indices[9] = 4; indices[10] = 7; indices[11] = 6;

	// ����
	indices[12] = 4; indices[13] = 5; indices[14] = 1;
	indices[15] = 4; indices[16] = 1; indices[17] = 0;

	// ����
	indices[18] = 3; indices[19] = 2; indices[20] = 6;
	indices[21] = 3; indices[22] = 6; indices[23] = 7;

	// ���
	indices[24] = 1; indices[25] = 5; indices[26] = 6;
	indices[27] = 1; indices[28] = 6; indices[29] = 2;

	// �ϴ�
	indices[30] = 4; indices[31] = 0; indices[32] = 3;
	indices[33] = 4; indices[34] = 3; indices[35] = 7;
	

	IB->Unlock();

	// ī�޶��� ��ġ�� ������ �����Ѵ�.
	D3DXVECTOR3 position(0.0f, 0.0f, -5.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);

	gpD3DDevice->SetTransform(D3DTS_VIEW, &V);

	// ���� ����� �����Ѵ�.
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f, // �þ߰� 90��
		(float)WIN_WIDTH/(float)WIN_HEIGHT,
		1.0f,
		1000.0f);
	gpD3DDevice->SetTransform(D3DTS_PROJECTION, &proj);

	gpD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			
	return true;
}

void Cleanup()
{
	// VB�� IB�� �޸𸮸� �����Ѵ�.
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
	d3d::Release<IDirect3DIndexBuffer9*>(IB);

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

bool InitD3D(HWND hWnd)
{

	gpD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (!gpD3D)
	{
		return false;
	}

	D3DCAPS9 caps;
	gpD3D->GetDeviceCaps(
		D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL,		 
		&caps				
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

	d3dpp.BackBufferWidth = WIN_WIDTH;
	d3dpp.BackBufferHeight = WIN_HEIGHT;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.Windowed = TRUE;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;
	d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (FAILED(gpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &gpD3DDevice)))
	{
		return false;
	}

	return true;

}
