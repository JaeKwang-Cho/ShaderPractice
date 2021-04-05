#include "d3_init.h"

// D3D ����
extern LPDIRECT3D9             gpD3D;				// D3D
extern LPDIRECT3DDEVICE9       gpD3DDevice;				// D3D ��ġ

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
bool Setup()
{
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

