#include "d3_stencil.h"

// D3D ����
extern LPDIRECT3D9             gpD3D;				// D3D
extern LPDIRECT3DDEVICE9       gpD3DDevice;				// D3D ��ġ

IDirect3DVertexBuffer9* VB = NULL;

// �𵨰���

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
	// �� �ݻ� ���ϰ�
	WallMtrl.Specular = d3d::WHITE * 0.2f;
	// �����ڸ� �����.
	D3DXCreateTeapot(gpD3DDevice, &Teapot, 0);

	//
	//   |----|----|----|
	//   |Wall|Mirr|Wall|
	//   |    | or |    |
	//   /--------------/
	//  /   Floor      /
	// /--------------/
	//
	// �̰� �����.

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

	// �ٴ�
	v[0] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

	v[3] = Vertex(-7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[4] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[5] = Vertex(7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	// ���� ��
	v[6] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[7] = Vertex(-7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[8] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[9] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[10] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[11] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// ������ ��
	v[12] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[13] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[14] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[15] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[16] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[17] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// �ſ�
	v[18] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[19] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[20] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[21] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[22] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[23] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	VB->Unlock();

	// �ؽ��� �ε�
	D3DXCreateTextureFromFile(gpD3DDevice, "checker.jpg", &FloorTex);
	D3DXCreateTextureFromFile(gpD3DDevice, "brick0.jpg", &WallTex);
	D3DXCreateTextureFromFile(gpD3DDevice, "ice.bmp", &MirrorTex);

	// ���� ����
	gpD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	gpD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	gpD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);


	// ���� �����ϱ�
	D3DXVECTOR3 lightDir(0.707f, -0.707f, 0.707f);
	D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f);
	D3DLIGHT9 light = d3d::InitDirectionalLight(&lightDir, &color);

	gpD3DDevice->SetLight(0, &light);
	gpD3DDevice->LightEnable(0, true);

	gpD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	gpD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, true);
	

	
	// �� ���
	// ī�޶��� ��ġ�� ������ �����Ѵ�.
	D3DXVECTOR3 pos(-10.0f, 3.0f, -15.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);

	gpD3DDevice->SetTransform(D3DTS_VIEW, &V);

	// ���� ��Ʈ������ �����Ѵ�.
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
		// ������Ʈ
		
		static float radius = 20.f;

		// ����Ű �Է��� �޾Ƽ�, ������ ��ġ�� �����δ�.
		if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
			TeapotPosition.x -= 3.0f * timeDelta;

		if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
			TeapotPosition.x += 3.0f * timeDelta;

		// �������� ũ�⸦ �����Ѵ�..
		if (::GetAsyncKeyState(VK_UP) & 0x8000f)
			radius -= 2.0f * timeDelta;

		if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
			radius += 2.0f * timeDelta;

		static float angle = (3.0f * D3DX_PI) / 2.0f;

		// Ű���� �Է��� �޾Ƽ�, ������ �����Ѵ�..
		if (::GetAsyncKeyState('A') & 0x8000f)
			angle -= 0.5f * timeDelta;

		if (::GetAsyncKeyState('S') & 0x8000f)
			angle += 0.5f * timeDelta;

		// x�� cos, z ��ǥ�� sin���� ����
		D3DXVECTOR3 position(cosf(angle) * radius, 3.0f, sinf(angle) * radius);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		// �޼� ��ǥ�� ��� ����. (z�� ȭ������� ���� Ŀ���� ��)
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		gpD3DDevice->SetTransform(D3DTS_VIEW, &V);
		
		//
		// Draw the scene:
		//

		// ���ٽ� ���۸� �ʱ�ȭ ���ش�.
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
	// ������ �׸���
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

	// VB�� �׸� �غ�
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	gpD3DDevice->SetTransform(D3DTS_WORLD, &I);

	gpD3DDevice->SetStreamSource(0, VB, 0, sizeof(Vertex));
	gpD3DDevice->SetFVF(Vertex::FVF);

	// �ٴ� �׸���
	gpD3DDevice->SetMaterial(&FloorMtrl);
	gpD3DDevice->SetTexture(0, FloorTex);
	gpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	// �� �׸���
	gpD3DDevice->SetMaterial(&WallMtrl);
	gpD3DDevice->SetTexture(0, WallTex);
	gpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 4);

	// �ſ� �׸���
	gpD3DDevice->SetMaterial(&MirrorMtrl);
	gpD3DDevice->SetTexture(0, MirrorTex);
	gpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);
	
}
void RenderMirror()
{ 
	gpD3DDevice->Clear(0, NULL, D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0L);

	// 1. ���ٽ� ���۸� Ȱ��ȭ �ϰ� ���õ� ���� ������ ���� �����Ѵ�.
	gpD3DDevice->SetRenderState(D3DRS_STENCILENABLE, true);
	// �׻� ���ٽ��� �����ϵ��� ��
	gpD3DDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	// ���ٽ� �׽�Ʈ ����ġ�� 1�� ����
	gpD3DDevice->SetRenderState(D3DRS_STENCILREF, 0x1);
	// ����ũ�� ���δ� �ѱ�
	gpD3DDevice->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	gpD3DDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	// ���� �׽�Ʈ�� ������ ��� ���ٽ� ���۸� �������� ����
	gpD3DDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	// ���ٽ� �׽�Ʈ�� ������ ��� ���� ���� (FUNC - ALWAYS�� ��� �ʿ����)
	gpD3DDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	// ���̿� ���ٽ� �׽�Ʈ�� �����ϸ� ����ġ�� 1�� �����Ѵ�.
	gpD3DDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

	// 2. ���ٽ� ���ۿ� �ſ��� ������ �Ѵ�.
	// �ϴ� ���ٽ� ���ۿ� ������ �ϹǷ�
	// ���� ���ۿ� �ĸ� ���۷� �������� ���� ���´�.
	gpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
	// ���� ����� true
	gpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	// ���� ���� �μ��� 0���� �Ѵ�.
	gpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
	// ������ ���� �μ��� 1���� �����Ѵ�.
	gpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	// �̷��� �ϸ� �ĸ���۰� �ٲ��� �ʴ´�.

	// ���ٽ� ���ۿ� �ſ��� �׸���.
	gpD3DDevice->SetStreamSource(0, VB, 0, sizeof(Vertex));
	gpD3DDevice->SetFVF(Vertex::FVF);
	gpD3DDevice->SetMaterial(&MirrorMtrl);
	gpD3DDevice->SetTexture(0, MirrorTex);
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	gpD3DDevice->SetTransform(D3DTS_WORLD, &I);
	// 18������ �ﰢ�� 2���� �׸���.
	gpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);

	// ���̹��� �ٽ� ���⸦ Ȱ��ȭ ��Ų��.
	gpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);

	// ���� ���ټ� ���ۿ��� ���̴� ȭ�鿡���̴� �ȼ� �� �ſ￡ �ش��ϴ� �ȼ�����
	// 0x1 ���� ������ �ſ�� ������ �� �κ��� ǥ���Ѵ�.

	// 3. �������¸� �ٽ� �����Ѵ�. �ſ￡����(0x1) �ݻ�ǵ��� ���� 
	gpD3DDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	// �׸��� �����ϸ� ���ٽ� ���� ���� �����ϵ��� ����
	gpD3DDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

	//4. �ݻ�� �����ڸ� ������ �ϱ� ���� ����� �Ѵ�.

	// �ݻ��� ��ġ�� ��´�.
	D3DXMATRIX W, T, R;
	D3DXPLANE plane(0.0f, 0.0f, 1.0f, 0.0f); // xy���
	D3DXMatrixReflect(&R, &plane); // ������ ��鿡 ���� ������Ű�� ����� ��ȯ

	D3DXMatrixTranslation(
		&T,
		TeapotPosition.x,
		TeapotPosition.y,
		TeapotPosition.z
	); // �������� ��ġ�� �̵��ϴ� ����� ��ȯ

	W = T * R; // �ſ￡ �ݻ�� �������� ��ġ ���

	// 5. �ݻ�� �����ڸ� �������ϱ� ���� �غ�

	// �ݻ�� �����ڴ� �ſﺸ�� �ڿ� �����Ƿ� �׷����� �ʴ´�.
	// ���� ���۸� �ϴ� �����ش�.
	gpD3DDevice->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	// �׸��� �ſ�� �ݻ�� �����ڸ� blend ���ش�.
	// �������� �ſ��̰� ������ �������̹Ƿ� ������ ���� �ۼ����ش�.

	// ������ ������ ����� �������ְ�
	gpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
	// �������� �׳� 0���� �Ѵ�.
	gpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	// 6. �ݻ�� ���� �����ڸ� �׸���.

	// �ݻ�� ������ ������ġ����� ���� ��Ű��
	gpD3DDevice->SetTransform(D3DTS_WORLD, &W);
	gpD3DDevice->SetMaterial(&TeapotMtrl);
	gpD3DDevice->SetTexture(0, 0);

	// �޸��� ���ؽ��� �׸��� �ʰ� �߶󳻴� ������, ��ȸ������ �Ѵ�.
	gpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	// ��ü�� ����� �ĸ��� �ڹٲ�� ������, ������ �������� CULLing�� ������ �ٲپ� �ش�.
	Teapot->DrawSubset(0);

	// �ٽ� ������ ���ٽ��� ��Ȱ��ȭ �ϰ�, culling�� ������ ������� �����Ѵ�.
	gpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	gpD3DDevice->SetRenderState(D3DRS_STENCILENABLE, false);
	gpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void Cleanup()
{
	// ���� ������ ���� ���ش�.
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
	d3d::Release<IDirect3DTexture9*>(FloorTex);
	d3d::Release<IDirect3DTexture9*>(WallTex);
	d3d::Release<IDirect3DTexture9*>(MirrorTex);
	d3d::Release<ID3DXMesh*>(Teapot);
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



