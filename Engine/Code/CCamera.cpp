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
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

	return S_OK;
}

_int CCamera::Update_GameObject(const _float& fTimeDelta)
{
	//LookAtLH();
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	return 0;
}

void CCamera::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CCamera::MakeTransformMatrix(const _vec3* pRight,
								const _vec3* pUp,
								const _vec3* pLook,
								const _vec3* pPos)
{
	memcpy(&m_matView.m[0][0], pRight, sizeof(_vec3));
	memcpy(&m_matView.m[1][0], pUp, sizeof(_vec3));
	memcpy(&m_matView.m[2][0], pLook, sizeof(_vec3));
	memcpy(&m_matView.m[3][0], pPos, sizeof(_vec3));
}

void CCamera::Free()
{
	CGameObject::Free();
}
