#include <d3d9.h>
#include <d3dx9.h>

/* ǥ�� (Surface) */
// ǥ�� ����
D3DSURFACE_DESC surfaceDESC; 
// IDirect3DSurface9 �������̽����� ->GetDesc�� ��´�.

// ǥ�� �ȼ� �����ͷ��� �����͸� ��´�.
D3DLOCKED_RECT lockedRect;
// IDirect3DSurface9 �������̽����� -> LockRect()�� ��´�.

/* ��Ƽ ���ø� */
D3DMULTISAMPLE_TYPE multiSamType;
// none �׸��� 1���� 16���� ������ ������ �� �ִ�.
// IDirect3D9::CheckDeviceMultiSampleType(); ���� �׷��� ī�忡�� �����ϴ� ��Ƽ ���ø� Ÿ�԰� ǰ�� ������ Ȯ���Ѵ�.

/* �ȼ� ���� */
D3DFORMAT format; \
// 24��Ʈ, 32��Ʈ (����), 64��Ʈ, 128 ��� ���� �ִ�
// �ñ��ϸ� ã�ƺ���.

/* �޸� Ǯ */
D3DPOOL pool1 = D3DPOOL_DEFAULT;  // �ݵ�� IDirect3DDevice9::reset ������ ���� �ؾ� �Ѵ�. �׸��� �ٽ� �ʱ�ȭ �Ѵ�.
D3DPOOL pool2 = D3DPOOL_MANAGED;
D3DPOOL pool3 = D3DPOOL_SYSTEMMEM;
D3DPOOL pool4 = D3DPOOL_SCRATCH;

/* ���� ü�� */
// IDirect3DSwapChain9 �������̽�: ���� ����.

/* ���� ���� */

/* ���ؽ� ���μ��� */
 
/* ��ġ Ư�� */
D3DCAPS9 devcaps;
bool supportsHardwareVertexProcessing = D3DDEVCAPS_HWTRANSFORMANDLIGHT; 
// ���������� �����Ѵ�.

