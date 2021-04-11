#include "d3_stencil.h"


LPDIRECT3D9             gpD3D = NULL;				// D3D
LPDIRECT3DDEVICE9       gpD3DDevice = NULL;				// D3D ��ġ

const char* gAppName = "����";

//-----------------------------------------------------------------------
// ���α׷� ������/�޽��� ����
//-----------------------------------------------------------------------


INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L,
					  GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
					  gAppName, NULL };
	RegisterClassEx(&wc);


	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	HWND hWnd = CreateWindow(gAppName, gAppName,
		style, CW_USEDEFAULT, 0, WIN_WIDTH, WIN_HEIGHT,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	POINT ptDiff;
	RECT rcClient, rcWindow;

	GetClientRect(hWnd, &rcClient);
	GetWindowRect(hWnd, &rcWindow);
	
	ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
	ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
	
	MoveWindow(hWnd, rcWindow.left, rcWindow.top, WIN_WIDTH + ptDiff.x, WIN_HEIGHT + ptDiff.y, TRUE);

	
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);

	
	if (!InitEverything(hWnd))
		PostQuitMessage(1);

	
	MSG msg;
	
	ZeroMemory(&msg, sizeof(msg));
	
	static float lastTime = (float)timeGetTime();
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			float currTime = (float)timeGetTime();
			float timeDelta = (currTime - lastTime) * 0.001f;

			Display(timeDelta);

			lastTime = currTime;
		}
	}

	
	UnregisterClass(gAppName, wc.hInstance);
	return 0;
}


LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
	
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


void ProcessInput(HWND hWnd, WPARAM keyPress)
{
	switch (keyPress)
	{
		
	case VK_ESCAPE:
		
		PostMessage(hWnd, WM_DESTROY, 0L, 0L);
		break;
	}
}


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
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

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

D3DMATERIAL9 d3d::InitMtrl(D3DXCOLOR ambient, D3DXCOLOR diffuse, D3DXCOLOR specular, D3DXCOLOR emissive, float power)
{
	D3DMATERIAL9 mtrl;

	mtrl.Ambient = ambient;
	mtrl.Diffuse = diffuse;
	mtrl.Specular = specular;
	mtrl.Emissive = emissive;
	mtrl.Power = power;

	return mtrl;
}
D3DLIGHT9 d3d::InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color)
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
D3DLIGHT9 d3d::InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color)
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
D3DLIGHT9 d3d::InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color)
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