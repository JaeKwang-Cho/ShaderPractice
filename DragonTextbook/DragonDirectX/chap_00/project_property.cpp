
#include "project_property.h"
#include <stdio.h>


void type_practice()
{
	/* Vector */
	D3DXVECTOR3 u(1.0f, 0.0f, 1.0f);
	D3DXVECTOR3 v(0.0f, 1.0f, 0.0f);

	// +=, -=, *=, /=, +, -,*, /, == !=
	// Internally use Epsilon
	
	float vec_length = D3DXVec3Length(&u);

	D3DXVECTOR3 nu;
	D3DXVec3Normalize(&nu, &u);
	D3DXVECTOR3 nv;
	D3DXVec3Normalize(&nv, &v);

	float dot = D3DXVec3Dot(&nu, &nv);
	D3DXVECTOR3 cross;
	D3DXVec3Cross(&cross, &nu, &nv);

	/* Matrix */
	D3DXMATRIX A(1.0, 2.0, 3.0, 0.0, 2.0, 3.0, 4.0, 0.0, 3.0, 4.0, 5.0, 0.0, 4.0, 5.0, 6.0, 0.0); // �ʱ�ȭ1
	float arrB[16] = { 1.0 };
	D3DXVECTOR4 B(arrB); // �ʱ�ȭ 2
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I); //d3dx9math.h

	D3DXMATRIX PA;
	D3DXMatrixTranspose(&PA, &A); // ��ġ ���

	D3DXVECTOR3 p1(2.0f, 4.0f, 2.0f);
	D3DXMATRIX tm;
	D3DXMatrixTranslation(&tm, 0.5f, 0.5f, 0.5f); // �̵����

	float pi = D3DX_PI;

	// �� ������ ȸ���ϴ� ���
	D3DXMATRIX XR;
	D3DXMatrixRotationX(&XR, pi / 4.0f);
	D3DXMATRIX YR;
	D3DXMatrixRotationY(&YR, pi / 4.0f);
	D3DXMATRIX ZR;
	D3DXMatrixRotationZ(&ZR, pi / 4.0f);

	// ũ�� ���� ���
	D3DXMATRIX SM;
	D3DXMatrixScaling(&SM, 2.0f, 2.0f, 2.0f);

	/* �߿� */
	D3DXVECTOR3 p2;
	D3DXVec3TransformCoord(&p2, &p1, &tm); // ���͸� ������ ��� ���� ���
	D3DXVECTOR3 p3;
	D3DXVec3TransformNormal(&p3, &p1, &tm); // ���͸� ���ͷ� ���

	// ��� �̰� �ξ� ���� ���� ����.
	// ->	D3DXVec3TransformCoordArray(); 
	// ->	D3DXVec3TransformNormalArray();

	// ���
	D3DXPLANE Q(1.0f, 1.0f, 1.0f, 3.0f); // ���� ���Ϳ� d ��
	// ���� ���Ͱ� ���� ���̸� -> (���� ������ �������� �Ÿ�) = (���� ����) dot (�� ����) + (d)

	// ������ ����
	D3DXPLANE P(0.0f, 1.0f, 0.0f, 0.0f);
	D3DXVECTOR3 V(3.0f, 5.0f, 2.0f);

	float x = D3DXPlaneDotCoord(&P, &V);
	if ((x - 0.0f) <= FLT_EPSILON);
	if (x > 0.0f);
	if (x < 0.0f);

	// ��� ����
	D3DXPLANE plane1;
	D3DXVECTOR3 normal(1.0f, 0.0f, 1.0f);
	D3DXVECTOR3 pointInPlane(2.0f, 1.0f, 3.0f);
	D3DXPlaneFromPointNormal(&plane1, &normal, &pointInPlane);

	D3DXPLANE plane2;
	D3DXVECTOR3 point1(1.0f,0.0f,0.0f);
	D3DXVECTOR3 point2(0.0f,1.0f,0.0f);
	D3DXVECTOR3 point3(0.0f,0.0f,1.0f);
	D3DXPlaneFromPoints(&plane2, &point1, &point2, &point3);

	// ����� ����ȭ
	D3DXPLANE norPlane;
	D3DXPlaneNormalize(&norPlane, &plane1);

	// ��� ��ȯ (�̰� �ñ��ϸ� ã�ƺ���)
	// �ϴ� X������ ȸ���ϴ� ��� ������
	D3DXMATRIX invXR;
	D3DXMATRIX invTransXR;

	D3DXMatrixInverse(&invXR, 0, &XR);
	D3DXMatrixTranspose(&invTransXR, &XR);

	D3DXPlaneNormalize(&plane2, &plane2);

	D3DXPlaneTransform(&plane2, &plane2, &invTransXR);

}





