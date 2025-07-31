#include "CCameraCom.h"

CCameraCom::CCameraCom(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
}

CCameraCom::~CCameraCom()
{
}

HRESULT CCameraCom::Ready_Camera()
{
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

	return S_OK;
}

_int CCameraCom::Update_Component(const _float& fTimeDelta)
{
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	return 0;
}

void CCameraCom::LateUpdate_Component()
{
}

void CCameraCom::MakeTransformMatrix(const _vec3* pRight, const _vec3* pUp, const _vec3* pLook, const _vec3* pPos)
{
	memcpy(&m_matView.m[0][0], pRight, sizeof(_vec3));
	memcpy(&m_matView.m[1][0], pUp, sizeof(_vec3));
	memcpy(&m_matView.m[2][0], pLook, sizeof(_vec3));
	memcpy(&m_matView.m[3][0], pPos, sizeof(_vec3));
}

CCameraCom* CCameraCom::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCameraCom* pCameraCom = new CCameraCom(pGraphicDev);

	if (FAILED(pCameraCom->Ready_Camera()))
	{
		Safe_Release(pCameraCom);
		MSG_BOX("CameraCom Create Failed");
		return nullptr;
	}

	return pCameraCom;
}

CComponent* CCameraCom::Clone()
{
	return new CCameraCom(*this);
}

void CCameraCom::Free()
{
	CComponent::Free();
}