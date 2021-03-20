//**********************************************************************
//
// ShaderFramework.cpp
// 
// ���̴� ���� ���� C��Ÿ���� �ʰ��� �����ӿ�ũ�Դϴ�.
// (���� ������ �ڵ��Ͻ� ���� ���� �̷��� �����ӿ�ũ��
// �ۼ��Ͻø� �ȵ˴ϴ�. -_-)
//
// Author: Pope Kim
//
//**********************************************************************

#include "ShaderFramework.h"
#include <stdio.h>

// ��������� ���� �� �ʿ��� ������� �ϴ� ����
#define PI				3.14159265f
#define FOV				(PI/4.0f)						// �þ߰�
#define ASPECT_RATIO	((WIN_WIDTH)/(float)WIN_HEIGHT)	// ȭ�� ��Ⱦ��
#define NEAR_PLANE		1								// ���� ���
#define FAR_PLANE		10000							// ���Ÿ� ���


//----------------------------------------------------------------------
// ��������
//----------------------------------------------------------------------

// D3D ����
LPDIRECT3D9             gpD3D				= NULL;				// D3D
LPDIRECT3DDEVICE9       gpD3DDevice			= NULL;			// D3D ��ġ

// ��Ʈ
ID3DXFont* gpFont = NULL;

// ��
LPD3DXMESH				gpTeapot			= NULL;
// ���̴�
LPD3DXEFFECT			gpEnvironmentMappingShader = NULL;
LPD3DXEFFECT			gpNoEffect	= NULL;
LPD3DXEFFECT			gpGrayScale = NULL;
LPD3DXEFFECT			gpSepia = NULL;
LPD3DXEFFECT			gpEdgeDetection = NULL;
LPD3DXEFFECT			gpEmboss = NULL;


// �ؽ�ó
LPDIRECT3DTEXTURE9		gpStoneDM			= NULL;
LPDIRECT3DTEXTURE9		gpStoneSM			= NULL;
LPDIRECT3DTEXTURE9		gpStoneNM			= NULL;
LPDIRECT3DCUBETEXTURE9	gpSnowENV			= NULL;

// ��� ����Ÿ��
LPDIRECT3DTEXTURE9		gpSceneRenderTarget = NULL;

// ȭ���� ���� ä��� �簢��
LPDIRECT3DVERTEXDECLARATION9	gpFullscreenQuadDecl = NULL; // ���� ���̴��� �ԷµǴ� ������ ������ ����
LPDIRECT3DVERTEXBUFFER9			gpFullscreenQuadVB = NULL; // ���� ���� ���ҽ��� �����ϴ� �������̽�
LPDIRECT3DINDEXBUFFER9			gpFullscreenQuadIB = NULL; // �ε��� ���� ���ҽ��� �����ϴ� �������̽�

// ���� ����
D3DXVECTOR4				gLightColor(0.7f, 0.7f, 1.0f, 1.0f);

// ���α׷� �̸�
const char* gAppName = "Picturing ���̴� �����ӿ�ũ";
// ȸ����
float gRotationY = 0.0f;
// ����� ����Ʈ���μ��� ���̴��� ����
int		gPostProcessIndex = 0;
// ���� ��ġ
D3DXVECTOR4				gWorldLightPosition(500.0f, 500.0f, -500.0f, 1.0f);
// ī�޶� ��ġ
D3DXVECTOR4				gWorldCameraPosition(0.0f, 0.0f, -200.0f, 1.0f);


//-----------------------------------------------------------------------
// ���α׷� ������/�޽��� ����
//-----------------------------------------------------------------------

// ������
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	// 1. ������ Ŭ������ ����Ѵ�.
	// ���α׷��� â�� �����ϱ� ���ؼ��� ���� ���� ������ Ŭ������ ����ؾ��Ѵ�.
	// 1) Ŭ���� ũ��, 2) ��Ÿ�� 3) ������ ���ν��� ������ 4) Ŭ���� �߰� �޸� 5) ������ �߰��޸�
	// 6) �ڵ鷯 7) ������ ���ҽ� ������ 8) Ŀ�� ���ҽ� ������ 9) �귯�� �ڵ� 10) �޴� �̸� ������
	// 11) Ŭ���� �̸� ������ 12) ������ ������
	// ��... �� �ϳ��ϳ��� �ñ��ϸ� ã�ƺ���
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
					  GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
					  gAppName, NULL };
	RegisterClassEx(&wc);

	// 2. ���α׷� â�� �����Ѵ�.
	// ������ Ŭ������ �ν��Ͻ��� �����Ѵ�.
	// ���� � ���ڸ� �ѱ���� �ñ��ϸ� ã�ƺ���
	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	HWND hWnd = CreateWindow(gAppName, gAppName,
		style, CW_USEDEFAULT, 0, WIN_WIDTH, WIN_HEIGHT,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	// 3. Client Rect ũ�Ⱑ WIN_WIDTH, WIN_HEIGHT�� ������ ũ�⸦ �����Ѵ�.
	POINT ptDiff;
	RECT rcClient, rcWindow;

	// ���� �������� ������ ������ Client Rect�̴�.
	GetClientRect(hWnd, &rcClient);
	GetWindowRect(hWnd, &rcWindow);
	// ������ Ʋ�� ©���� �κ��� ��ǥ�� ���ϰ�
	ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
	ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
	// �����Ѵ�.
	MoveWindow(hWnd, rcWindow.left, rcWindow.top, WIN_WIDTH + ptDiff.x, WIN_HEIGHT + ptDiff.y, TRUE);

	// 4. ���� â�� �����ش�.
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);

	// 5. D3D�� ����� ��� ���� �ʱ�ȭ�Ѵ�.
	if (!InitEverything(hWnd))
		PostQuitMessage(1);

	// 6. �޽��� ����
	MSG msg;
	// ������ ��ũ�� �Լ� �޸𸮸� 0�� �ʱ�ȭ
	ZeroMemory(&msg, sizeof(msg));
	// WM_QUIT �ޱ� ������ ��� ������.
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else // �޽����� ������ ������ ������Ʈ�ϰ� ����� �׸���
		{
			PlayDemo();
		}
	}
	// �̰͵� ���� ��ũ���Լ��̰�, ������ Ŭ������ ����� �����Ѵ�.
	UnregisterClass(gAppName, wc.hInstance);
	return 0;
}

// �޽��� ó����
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
	// ó������ �ʴ� �޽����� default �⺻ ó��
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Ű���� �Է�ó��
void ProcessInput(HWND hWnd, WPARAM keyPress)
{
	switch (keyPress)
	{
		// ESC Ű�� ������ ���α׷��� �����Ѵ�.
	case VK_ESCAPE:
		// ������ �ڵ鿡�� �޽����� ������ ��ũ���Լ�
		PostMessage(hWnd, WM_DESTROY, 0L, 0L);
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
		gPostProcessIndex = keyPress - '0' - 1;
		break;
	}
}

//------------------------------------------------------------
// ���ӷ���
//------------------------------------------------------------
void PlayDemo()
{
	Update();
	RenderFrame();
}

// ���ӷ��� ������Ʈ
void Update()
{
}

//------------------------------------------------------------
// ������
//------------------------------------------------------------
	
void RenderFrame()
{
	D3DCOLOR bgColour = 0x00000000;	// ������ - ����

	// ����ϰ� ���� �����, ����/���ٽ��� clear �Ѵ�.
	gpD3DDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), bgColour, 1.0f, 0);

	// ���� �׸��� �����Ѵ�.
	gpD3DDevice->BeginScene();
	{
		RenderScene();				// 3D ��ü���� �׸���.
		RenderInfo();				// ����� ���� ���� ����Ѵ�.
	}
	// ���� �� �׷�����
	gpD3DDevice->EndScene();

	// ����ۿ� �ִ� ���� ȭ�鿡 �Ѹ���.
	gpD3DDevice->Present(NULL, NULL, NULL, NULL);
}


// 3D ��ü���� �׸���.
void RenderScene()
{
	/////////////
	// 1. ����� ����Ÿ�� �ȿ� �׸���.
	/////////////

	// ���� �ϵ���� �����
	LPDIRECT3DSURFACE9 pHWBackBuffer = NULL;
	gpD3DDevice->GetRenderTarget(0, &pHWBackBuffer);

	// ����Ÿ�� ���� �׸���.
	LPDIRECT3DSURFACE9 pSceneSurface = NULL;
	if (SUCCEEDED(gpSceneRenderTarget->GetSurfaceLevel(0, &pSceneSurface)))
	{
		gpD3DDevice->SetRenderTarget(0, pSceneSurface);
		pSceneSurface->Release();
		pSceneSurface = NULL;
	}
	// ���� ���۸� ���� ������� �ʴ� ������ ����Ÿ���� ũ�Ⱑ �ϵ���� ���� ������ ũ��� �����ϱ� ����

	// ���� �����ӿ� �׷ȴ� ����� �����.
	gpD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0xFF000000, 1.0f, 0);
	// �̷��� �ϸ� ��� ������ ����� ����Ÿ�� �ȿ� ����.

	// �� ����� �����.
	D3DXMATRIXA16 matView;
	D3DXVECTOR3 vEyePt(gWorldCameraPosition.x, gWorldCameraPosition.y, gWorldCameraPosition.z);	// ī�޶��� ��ġ ����
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);	// �ٶ󺸴� ���� ��ġ ����
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);		// ī�޶� ���� ����Ű�� ����

	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);

	// ���� ����� �����. (���� ���ù�)
	D3DXMATRIXA16 matProjection;
	// ������ �����ߴ� ������ �־��ش�.
	D3DXMatrixPerspectiveFovLH(&matProjection, FOV, ASPECT_RATIO, NEAR_PLANE, FAR_PLANE);
	
	// ������ y�� �߽����� ���ݾ� ȸ�� ��Ų��
	gRotationY += 0.4f * PI / 180.0f;
	if (gRotationY > 2 * PI)
	{
		gRotationY -= 2 * PI;
	}

	// ���� ����� �����.
	// ���� ����� 1. �� ��ü�� ��ġ�� ���� 2.Ȯ��/��� ��ȯ�� ��ģ ���̴�.
	// *** ����� �� ������İ� �޸� �� ��ü���� ��������� ����� �־���Ѵ�.
	D3DXMATRIXA16 matWorld;
	D3DXMatrixRotationY(&matWorld,gRotationY); // y������ ������ ���� ����� �����.

	// ����/��/��������� �̸� ���Ѵ�
	D3DXMATRIXA16	matWorldView;
	D3DXMATRIXA16	matWorldViewProjection;
	D3DXMatrixMultiply(&matWorldView, &matWorld, &matView);
	D3DXMatrixMultiply(&matWorldViewProjection, &matWorldView, &matProjection);

	gpEnvironmentMappingShader->SetMatrix("gWorldMatrix", &matWorld);
	gpEnvironmentMappingShader->SetMatrix("gWorldViewProjectionMatrix", &matWorldViewProjection);

	gpEnvironmentMappingShader->SetVector("gWorldLightPosition", &gWorldLightPosition);
	gpEnvironmentMappingShader->SetVector("gWorldCameraPosition", &gWorldCameraPosition);

	gpEnvironmentMappingShader->SetVector("gLightColor", &gLightColor);
	gpEnvironmentMappingShader->SetTexture("DiffuseMap_Tex", gpStoneDM);
	gpEnvironmentMappingShader->SetTexture("SpecularMap_Tex", gpStoneSM);
	gpEnvironmentMappingShader->SetTexture("DiffuseMap_Tex", gpStoneNM);
	gpEnvironmentMappingShader->SetTexture("EnvironmentMap_Tex",gpSnowENV);



	// ���� ���̴��� ���� ������ �׸� ��ü�鿡 �����϶�� GPU���� �����Ѵ�.
	// ���� �Լ����� �� ����
	UINT numPasses = 0;
	gpEnvironmentMappingShader->Begin(&numPasses, NULL); // Begin()�� End() ���̿�
	{
		for (UINT i = 0; i < numPasses; ++i)
		{
			gpEnvironmentMappingShader->BeginPass(i); // BeginPass()�� EndPass() �� �ְ�
			{
				// ��ü�� �׸���.
				gpTeapot->DrawSubset(0); // �� ���̿� .x�� �׸��� DrawSubset()�� �ִ�.
			}
			gpEnvironmentMappingShader->EndPass();
		}
	}
	gpEnvironmentMappingShader->End();
	// �̷��� �ϸ� GPU�� gpColorShader�� �̿��ؼ� gpSphere�� �׸���.

	///////////////
	//2. ����Ʈ ���μ����� �����Ѵ�.
	///////////////

	// �ϵ���� ����۸� �ٽ� ����Ѵ�.
	gpD3DDevice->SetRenderTarget(0, pHWBackBuffer);
	pHWBackBuffer->Release();
	pHWBackBuffer = NULL;

	// ����� ����Ʈ������ ȿ��
	LPD3DXEFFECT effectToUse = gpNoEffect;
	if (gPostProcessIndex == 1)
	{
		effectToUse = gpGrayScale;
	}
	else if (gPostProcessIndex == 2)
	{
		effectToUse = gpSepia;
	}
	else if (gPostProcessIndex == 3)
	{
		effectToUse = gpEdgeDetection;
	}
	else if (gPostProcessIndex == 4)
	{
		effectToUse = gpEmboss;
	}

	D3DXVECTOR4 pixelOffset(1 / (float)WIN_WIDTH, 1 / (float)WIN_HEIGHT, 0, 0);
	if (effectToUse == gpEdgeDetection || effectToUse == gpEmboss)
	{
		effectToUse->SetVector("gPixelOffset", &pixelOffset);
	}

	// �ؽ��ĸ� �����ϰ�
	// pass�� �����Ѵ�.
	effectToUse->SetTexture("SceneTexture_Tex", gpSceneRenderTarget);
	effectToUse->Begin(&numPasses, NULL);
	{
		for (UINT i = 0; i < numPasses; ++i)
		{
			// ȭ�� ���� �簢���� �׸���.
			effectToUse->BeginPass(i);
			{
				// �츮�� �׸� �簢���� D3DXMESH�� �ƴϿ���
				// DrawSubet() �Լ��� �ϴ����� ���� ���־�� �Ѵ�.
				// 1) ���� ���۸� D3D�� �����Ѵ�.
				// 2) ���� ���� ������ D3D�� �����Ͽ� ���� ������ �˷��ش�.
				// 3) �ε��� ���۸� D3D ��ġ�� �����Ѵ�.
				// 4) �׸��� �Լ��� ȣ���Ѵ�.

				gpD3DDevice->SetStreamSource(0, gpFullscreenQuadVB, 0, sizeof(float) * 5);
				// �ε��� ���ۿ� ���������� ����
				gpD3DDevice->SetIndices(gpFullscreenQuadIB);
				gpD3DDevice->SetVertexDeclaration(gpFullscreenQuadDecl);

				// ���������� �׸��� �Լ�
				// 1) ���� �����͸� �ﰢ������Ʈ�� ó���ϴ� �̳�
				// 2) ù��° �������� �׸���.
				// 3) �ּ� ������ 0
				// 4) �� 6���� ����
				// 5) ù��° ���� ���� ���
				// 6) �� 2���� �ﰢ���� �׸���.
				gpD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 6, 0, 2);
			}
			effectToUse->EndPass();
		}
	}
	effectToUse->End();
}

// ����� ���� ���� ���.
void RenderInfo()
{
	// �ؽ�Ʈ ����
	D3DCOLOR fontColor = D3DCOLOR_ARGB(255, 255, 255, 255);

	// �ؽ�Ʈ�� ����� ��ġ
	RECT rct;
	rct.left = 5;
	rct.right = WIN_WIDTH / 3;
	rct.top = 5;
	rct.bottom = WIN_HEIGHT / 3;

	// Ű �Է� ������ ���
	gpFont->DrawText(NULL, "���� �����ӿ�ũ\n\nESC: ��������\n1: Į��\n2: ���\n3: ���Ǿ�\n4: �ܰ���\n5: �簢ȿ��", -1, &rct, 0, fontColor);
}

//------------------------------------------------------------
// �ʱ�ȭ �ڵ�
//------------------------------------------------------------
bool InitEverything(HWND hWnd)
{
	// D3D�� �ʱ�ȭ
	if (!InitD3D(hWnd))
	{
		return false;
	}
	// ȭ���� ���� ä��� �簢���� �ϳ� �����.
	InitFullScreenQuad();
	
	// ����Ÿ���� �����.
	if (FAILED(gpD3DDevice->CreateTexture(WIN_WIDTH, WIN_HEIGHT,
		1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8,
		D3DPOOL_DEFAULT, &gpSceneRenderTarget, NULL)))
	{
		return false;
	}

	// ��, ���̴�, �ؽ�ó���� �ε�
	if (!LoadAssets())
	{
		return false;
	}

	// ��Ʈ�� �ε�
	// 1) D3D ����̽� 2) ��Ʈ ���� 3) ��Ʈ �ʺ� 4) ����ü 5) mipmap ���� 6) ���ڸ�ü ���� 7) ����set 8) ���� �����Ʈ�� ������ �Ӽ��� ������� ����ؾ��ϴ��� ����
	// 9) ���� �����Ʈ�� ǰ���� ������� ������ �ϴ��� ���� 8) ��ġ(������)�� �йи�(����) ���� 9) ��Ʈ �̸� 10) ��Ʈ ������ out ������
	if (FAILED(D3DXCreateFont(gpD3DDevice, 20, 10, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, (DEFAULT_PITCH | FF_DONTCARE),
		"Arial", &gpFont)))
	{
		return false;
	}

	return true;
}

// D3D ��ü �� ��ġ �ʱ�ȭ
bool InitD3D(HWND hWnd)
{
	// D3D ��ü
	gpD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!gpD3D)
	{
		return false;
	}


	// D3D��ġ�� �����ϴµ� �ʿ��� ����ü�� ä���ִ´�.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.BackBufferWidth = WIN_WIDTH;	// ������ ����, ������� �ʺ�
	d3dpp.BackBufferHeight = WIN_HEIGHT;	// ������� ����
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;// ������� ����
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;// ����۸� ������ ���� ȿ��. ���ɻ� �̰� ����.
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.Windowed = TRUE;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8; // ����/���ٽ� ������ ����
	d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // ����� �ֻ����� ����۸� �����ϴ� �󵵰��� ����. �̰��� ����Ͱ� ���� ����� ������ ����۸� �������ش�.
	// ���ӿ����� ���ɻ� ������� ���� ���⸦ ��ٸ��� �ʰ� ��ٷ� ������ �ִ� ��찡 ����.(D3DPRESENT IMMEDIATE) ��, �̷� ���� �� �����Ӱ� ���� �������� ���� ���ܺ��̴� ���ۿ��� �ִ�.

	// �� ����(D3DPRESENT_PARAMETERS ����ü)�� ������ D3D��ġ�� �����Ѵ�.
	if (FAILED(gpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &gpD3DDevice)))
	{
		return false;
	}

	return true;
}

bool LoadAssets()
{
	// �ؽ�ó �ε�
	gpStoneDM = LoadTexture("fieldstone_DM.tga");
	if (!gpStoneDM)
	{
		return false;
	}

	gpStoneSM = LoadTexture("fieldstone_SM.tga");
	if (!gpStoneSM)
	{
		return false;
	}

	gpStoneNM = LoadTexture("fieldstone_NM.tga");
	if (!gpStoneNM)
	{
		return false;
	}

	D3DXCreateCubeTextureFromFile(gpD3DDevice, "Snow_ENV.dds", &gpSnowENV);
	if (!gpSnowENV)
	{
		return false;
	}

	// ���̴� �ε�
	gpEnvironmentMappingShader = LoadShader("EnvironmentMapping.fx");
	if (!gpEnvironmentMappingShader)
	{
		return false;
	}

	gpNoEffect = LoadShader("NoEffect.fx");
	if (!gpNoEffect)
	{
		return false;
	}

	gpGrayScale = LoadShader("Grayscale.fx");
	if (!gpGrayScale)
	{
		return false;
	}

	gpSepia = LoadShader("Sepia.fx");
	if (!gpSepia)
	{
		return false;
	}

	gpEdgeDetection = LoadShader("EdgeDetection.fx");
	if (!gpEdgeDetection)
	{
		return false;
	}

	gpEmboss = LoadShader("Emboss.fx");
	if (!gpEmboss)
	{
		return false;
	}

	// �� �ε�
	gpTeapot = LoadModel("TeapotWithTangent.x");
	if (!gpTeapot)
	{
		return false;
	}

	return true;
}
// ���̴� �ε�
// .fx �������� ����� ���̴� ����
// �������̴��� �ȼ����̴��� ��� �����ϰ� �ִ� �ؽ�Ʈ �����̴�.
LPD3DXEFFECT LoadShader(const char* filename)
{
	LPD3DXEFFECT ret = NULL;

	LPD3DXBUFFER pError = NULL;
	DWORD dwShaderFlags = 0;

#if _DEBUG
	dwShaderFlags |= D3DXSHADER_DEBUG;
#endif

	// .fx ������ �ε� �� ������ �Ѵ�.
	// 1) D3D ����̽� 2. ���̴� ���� �̸� 3. ���̴��� ������ �� �� �߰��� ����� ��ó��#define
	// 4) #include ���ù��� ó���� �� ����� �������̽� ������ 5) ���̴��� �������� �� ����� �÷���
	// 6) �Ű����� ������ ����� ����Ʈ Ǯ 7) �ε��� ����Ʈ�� ������ ������ 8) ������ ���� �޽����� ����Ű�� ������
	D3DXCreateEffectFromFile(gpD3DDevice, filename,
		NULL, NULL, dwShaderFlags, NULL, &ret, &pError);

	// ���̴� �ε��� ������ ��� outputâ�� ���̴�
	// ������ ������ ����Ѵ�.
	if (!ret && pError)
	{
		int size = pError->GetBufferSize();
		void* ack = pError->GetBufferPointer();

		if (ack)
		{
			char* str = new char[size];
#pragma warning(suppress : 4996)
			sprintf(str, (const char*)ack, size);
			OutputDebugString(str);
			delete[] str;
		}
	}

	return ret;
}

// �� �ε�
// .x �������� ����� ���� �ε�
// DirectX���� ��ü������ �����ϴ� �޽� �����̴�.
LPD3DXMESH LoadModel(const char* filename)
{
	LPD3DXMESH ret = NULL;
	// 1) ���ϸ� 2) �ý��� �޸𸮿� �޽��� �ε��ϱ�� ���� 3) D3D ��ġ 4) ���� �ﰢ�� ������
	// 5) ���׸��� ���� 6) ����Ʈ �ν��Ͻ� 7) ���׸��� �� 8) �ε��� �޽��� ������ ������
	if (FAILED(D3DXLoadMeshFromX(filename, D3DXMESH_SYSTEMMEM, gpD3DDevice, NULL, NULL, NULL, NULL, &ret)))
	{
		OutputDebugString("�� �ε� ����: ");
		OutputDebugString(filename);
		OutputDebugString("\n");
	};

	return ret;
}

// �ؽ�ó �ε�
// �پ��� �������� ����� �̹������� �ؽ�ó�� �ε�
LPDIRECT3DTEXTURE9 LoadTexture(const char* filename)
{
	LPDIRECT3DTEXTURE9 ret = NULL;
	// 1) D3D ��ġ 2) ���ϸ� 3) �ؽ��� ����������
	if (FAILED(D3DXCreateTextureFromFile(gpD3DDevice, filename, &ret)))
	{
		OutputDebugString("�ؽ�ó �ε� ����: ");
		OutputDebugString(filename);
		OutputDebugString("\n");
	}

	return ret;
}
//------------------------------------------------------------
// ������ �ڵ�.
//------------------------------------------------------------

// �޸� ������ �����ϱ� ���� �����ߴ� �ڿ����� ������ ���� ���־�� �Ѵ�.
// ��ü�鵵 �� �������ش�.
void Cleanup()
{
	// ��Ʈ�� release �Ѵ�.
	if (gpFont)
	{
		gpFont->Release();
		gpFont = NULL;
	}

	// ���� release �Ѵ�.
	if (gpTeapot)
	{
		gpTeapot->Release();
		gpTeapot = NULL;
	}

	// ���̴��� release �Ѵ�.
	if (gpEnvironmentMappingShader)
	{
		gpEnvironmentMappingShader->Release();
		gpEnvironmentMappingShader = NULL;
	}

	if (gpNoEffect)
	{
		gpNoEffect->Release();
		gpNoEffect = NULL;
	}

	if (gpGrayScale)
	{
		gpGrayScale->Release();
		gpGrayScale = NULL;
	}

	if (gpSepia)
	{
		gpSepia->Release();
		gpSepia = NULL;
	}
	if (gpEdgeDetection)
	{
		gpEdgeDetection->Release();
		gpEdgeDetection = NULL;
	}
	if (gpEmboss)
	{
		gpEmboss->Release();
		gpEmboss = NULL;
	}

	// �ؽ�ó�� release �Ѵ�.
	if (gpStoneDM)
	{
		gpStoneDM->Release();
		gpStoneDM = NULL;
	}

	if (gpStoneSM)
	{
		gpStoneSM->Release();
		gpStoneSM = NULL;
	}

	if (gpStoneNM)
	{
		gpStoneNM->Release();
		gpStoneNM = NULL;
	}

	if (gpSnowENV)
	{
		gpSnowENV->Release();
		gpSnowENV = NULL;
	}

	// ȭ��ũ�� �簢���� �����Ѵ�
	if (gpFullscreenQuadDecl)
	{
		gpFullscreenQuadDecl->Release();
		gpFullscreenQuadDecl = NULL;
	}

	if (gpFullscreenQuadVB)
	{
		gpFullscreenQuadVB->Release();
		gpFullscreenQuadVB = NULL;
	}

	if (gpFullscreenQuadIB)
	{
		gpFullscreenQuadIB->Release();
		gpFullscreenQuadIB = NULL;
	}

	//����Ÿ���� �����Ѵ�
	if (gpSceneRenderTarget)
	{
		gpSceneRenderTarget->Release();
		gpSceneRenderTarget = NULL;
	}

	// D3D�� release �Ѵ�.
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

void InitFullScreenQuad()
{
	// ���� ������ �����.
	D3DVERTEXELEMENT9 vtxDesc[3]; // ���� ������ ���� ����ü

	// ù��° ����� ��ġ�� ����
	int offset = 0;
	int i = 0;

	// ��ġ
	vtxDesc[i].Stream = 0; // D3D ��ġ�� ���� ���۵��� ���ÿ� ������ ���� �� �ִµ�, �׶� ������ �ε����̴�.
	vtxDesc[i].Offset = offset; // ���������� �����ϴ� �޸� ��ġ���� ���� ��Ұ� �����ϴ� �������� ������ ���Դϴ�. 
	vtxDesc[i].Type = D3DDECLTYPE_FLOAT3; // ������ ���Դϴ�. ���� �����̱⿡ float3
	vtxDesc[i].Method = D3DDECLMETHOD_DEFAULT; // �ñ��ϸ� ã�ƺ���. ���� �̰ž���.
	vtxDesc[i].Usage = D3DDECLUSAGE_POSITION; // �뵵�� �����Ѵ�.
	vtxDesc[i].UsageIndex = 0; // �ø�ƽ �ε��� (�� �ڿ� �ٿ��� ���ڸ� ���Ѵ�.)

	// �ι�° ����� UV ��ǥ�� ����
	offset += sizeof(float) * 3;
	++i;

	// UV ��ǥ 0
	vtxDesc[i].Stream = 0;
	vtxDesc[i].Offset = offset;
	vtxDesc[i].Type = D3DDECLTYPE_FLOAT2; // UV ��ǥ�̹Ƿ� float2
	vtxDesc[i].Method = D3DDECLMETHOD_DEFAULT;
	vtxDesc[i].Usage = D3DDECLUSAGE_TEXCOORD; // UV ��ǥ�̹Ƿ� TEXCOORD
	vtxDesc[i].UsageIndex = 0;

	// ��� ���� ��ҵ��� �� ���ߴٴ� ǥ��
	offset += sizeof(float) * 2;
	++i;

	// ���������� ������ ǥ�� (D3DDECL_END())
	vtxDesc[i].Stream = 0xFF;
	vtxDesc[i].Offset = 0;
	vtxDesc[i].Type = D3DDECLTYPE_UNUSED;
	vtxDesc[i].Method = 0;
	vtxDesc[i].Usage = 0;
	vtxDesc[i].UsageIndex = 0;

	//���� �� �迭�� ������, D3D ��ġ�� ���� ������ ������
	gpD3DDevice->CreateVertexDeclaration(vtxDesc, &gpFullscreenQuadDecl);
	// ���� ���� ������, ���߿� mesh�� �׸��� D3D���� ���������� ���˿� ���缭 �ùٸ� �����͸� �̾ư����� �ϴµ��� ���δ�.

	// ���� ���۸� �����.
	// 1) ���� ������ ũ��, ������ �� ������ ���� ����Ʈ ���� �ְ�, 4�� ���� ���� ���̱� ������ 4�� �����ش�.
	// 2) Ư���� �뵵�� ������� �ʴ´�.
	// 3) FVF(flexible vertex format)�� �����ϴ� ���̴�.
	// 4) D3D�� �˾Ƽ� �������ִ� �޸� Ǯ�� ����Ѵ�.
	// 5) ����� ������
	// 6) ������ NULL
	gpD3DDevice->CreateVertexBuffer(offset * 4, 0, 0, D3DPOOL_MANAGED, &gpFullscreenQuadVB, NULL);

	// ���� ���ۿ� ������ ä�� �ִ´�.
	void* vertexData = NULL;
	// �ε��� �������� ������ ��׾�, �ε��� ���� �޸��� �����͸� ���´�.
	gpFullscreenQuadVB->Lock(0, 0, &vertexData, 0);
	{
		// �޸𸮿� ���� ������ ���ʴ�� ��ִ´�.
		float* data = (float*)vertexData;
		*data++ = -1.0f;	*data++ = 1.0f;		*data++ = 0.0f; // ����
		*data++ = 0.0f;		*data++ = 0.0f;						// UV

		*data++ = 1.0f;		*data++ = 1.0f;		*data++ = 0.0f;
		*data++ = 1.0f;		*data++ = 0;

		*data++ = 1.0f;		*data++ = -1.0f;	*data++ = 0.0f;
		*data++ = 1.0f;		*data++ = 1.0f;

		*data++ = -1.0f;	*data++ = -1.0f;	*data++ = 0.0f;
		*data++ = 0.0f;		*data++ = 1.0f;
	}
	gpFullscreenQuadVB->Unlock(); // ���� Ǯ���ش�

	// �ε��� ���۸� ������ش�
	// 1) ���� ������ ũ�� �ϴ� 16��Ʈ 6���� ���ڴ�.
	// 2) Ư���� �뵵�� ������� �ʴ´�.
	// 3) �� ������ 16��Ʈ���� ����
	// 4) D3D�� �˾Ƽ� �������ִ� �޸� Ǯ ���
	// 5) ����������
	// 6) �׻� ��
	gpD3DDevice->CreateIndexBuffer(sizeof(short) * 6, 0, D3DFMT_INDEX16,
		D3DPOOL_MANAGED, &gpFullscreenQuadIB, NULL);

	// ����� ������� �ε��� ���ۿ� �����͸� ����.
	void* indexData = NULL;
	gpFullscreenQuadIB->Lock(0, 0, &indexData, 0);
	{
		unsigned short* data = (unsigned short*)indexData;
		// �ð�������� �ﰢ���� 2�� ������ش�.
		*data++ = 0;	*data++ = 1;	*data++ = 3;
		*data++ = 3;	*data++ = 1;	*data++ = 2;
	}
	gpFullscreenQuadIB->Unlock();

	// �̷��� ���̴�.
}