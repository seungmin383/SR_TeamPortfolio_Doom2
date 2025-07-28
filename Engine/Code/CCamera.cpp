#include "CCamera.h"

CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CCamera::~CCamera()
{
}

HRESULT CCamera::Ready_GameObject()
{
	//D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	LookAtLH();
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	//D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
	PerspectiveFovLH();
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

	return S_OK;
}

_int CCamera::Update_GameObject(const _float& fTimeDelta)
{
	//D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	LookAtLH();
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	return 0;
}

void CCamera::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CCamera::LookAtLH()
{
	D3DXMatrixIdentity(&m_matView);

	_vec3 vInfo[INFO_END];
	ZeroMemory(&vInfo, sizeof(vInfo));

	vInfo[INFO_LOOK] = m_vAt - m_vEye;
	D3DXVec3Cross(&vInfo[INFO_RIGHT], &m_vUp, &vInfo[INFO_LOOK]);
	D3DXVec3Cross(&vInfo[INFO_UP], &vInfo[INFO_LOOK], &vInfo[INFO_RIGHT]);
	vInfo[INFO_POS] = m_vEye;

	D3DXVec3Normalize(&vInfo[INFO_LOOK], &vInfo[INFO_LOOK]);
	D3DXVec3Normalize(&vInfo[INFO_RIGHT], &vInfo[INFO_RIGHT]);
	D3DXVec3Normalize(&vInfo[INFO_UP], &vInfo[INFO_UP]);

	_float x = -D3DXVec3Dot(&vInfo[INFO_RIGHT], &vInfo[INFO_POS]);
	_float y = -D3DXVec3Dot(&vInfo[INFO_UP], &vInfo[INFO_POS]);
	_float z = -D3DXVec3Dot(&vInfo[INFO_LOOK], &vInfo[INFO_POS]);

	(m_matView)(0, 0) = vInfo[INFO_RIGHT].x;
	(m_matView)(0, 1) = vInfo[INFO_UP].x;
	(m_matView)(0, 2) = vInfo[INFO_LOOK].x;
	(m_matView)(0, 3) = 0.f;

	(m_matView)(1, 0) = vInfo[INFO_RIGHT].y;
	(m_matView)(1, 1) = vInfo[INFO_UP].y;
	(m_matView)(1, 2) = vInfo[INFO_LOOK].y;
	(m_matView)(1, 3) = 0.f;

	(m_matView)(2, 0) = vInfo[INFO_RIGHT].z;
	(m_matView)(2, 1) = vInfo[INFO_UP].z;
	(m_matView)(2, 2) = vInfo[INFO_LOOK].z;
	(m_matView)(2, 3) = 0.f;

	(m_matView)(3, 0) = x;
	(m_matView)(3, 1) = y;
	(m_matView)(3, 2) = z;
	(m_matView)(3, 3) = 1.f;
}

void CCamera::PerspectiveFovLH()
{
	D3DXMatrixIdentity(&m_matProj);

	_float A = m_fNear / (m_fFar - m_fNear);
	_float B = m_fNear * m_fFar / (m_fFar - m_fNear);

	_float H = 1 / tanf(m_fFov * 0.5f);
	_float W = H / m_fAspect;

	m_matProj._11 = W;
	m_matProj._22 = H;

	m_matProj._33 = A;
	m_matProj._43 = B;

	m_matProj._34 = 1.f;
	m_matProj._44 = 0.f;
}

void CCamera::Free()
{
	CGameObject::Free();
}
