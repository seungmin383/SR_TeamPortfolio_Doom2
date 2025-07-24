#include "CCamera.h"

CCamera::CCamera()
	: m_vEye(0.f, 10.f, -10.f),
	m_vAt(-5.f, 0.f, 0.f),
	m_vUp(0.f, 5.f, 0.f)
{
	ZeroMemory(&m_vInfo, sizeof(m_vInfo));
	D3DXMatrixIdentity(&m_matView);
}

CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev),
	m_vEye(0.f, 10.f, -10.f),
	m_vAt(-5.f, 0.f, 0.f),
	m_vUp(0.f, 5.f, 0.f)
{
	ZeroMemory(&m_vInfo, sizeof(m_vInfo));
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
}

CCamera::CCamera(const CCamera& rhs)
	: CComponent(rhs),
	m_vEye(rhs.m_vEye),
	m_vAt(rhs.m_vAt),
	m_vUp(rhs.m_vUp),
	m_matView(rhs.m_matView),
	m_matProj(rhs.m_matProj)
{
	for (size_t i = 0; i < INFO_END; i++)
	{
		m_vInfo[i] = rhs.m_vInfo[i];
	}
}

CCamera::~CCamera()
{
}

HRESULT CCamera::Ready_Camera()
{
	m_vUp = { 0.f, 1.f, 0.f };

	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);

	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	D3DXMatrixPerspectiveFovLH(&m_matProj, D3DXToRadian(60.f), float(WINCX) / WINCY, 0.1f, 1000.f);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

	return S_OK;
}

_int CCamera::Update_Component(const _float& fTimeDelta)
{
	return 0;
}

void CCamera::LateUpdate_Component()
{
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);

	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	D3DXMatrixPerspectiveFovLH(&m_matProj, D3DXToRadian(60.f), float(WINCX) / WINCY, 0.1f, 1000.f);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);
}

void CCamera::Set_Distance(const _float _fDistance)
{
}

void CCamera::Chase_Target(const _vec3* pTargetPos)
{
	m_vEye = { (*pTargetPos).x, (*pTargetPos).y + 1, (*pTargetPos).z - 1};
	m_vAt = { m_vEye.x, m_vEye.y - 5, m_vEye.z + 10};
}

void CCamera::Look_AroundX(const _float _fVal)
{
	//m_vAt = { m_vAt.x + _fVal, m_vAt.y, m_vAt.z };
	m_vAt.x = _fVal;
}

void CCamera::Look_AroundY(const _float _fVal)
{
	m_vAt.y = _fVal - 5;
}

void CCamera::Rotaion(ROTATION eType, const _float _angle)
{

}

CComponent* CCamera::Clone()
{
	return new CCamera(*this);
}

CCamera* CCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCamera* pCamera = new CCamera(pGraphicDev);
	if (FAILED(pCamera->Ready_Camera()))
	{
		Safe_Release(pCamera);
		MSG_BOX("Create Camera Failed");
		return nullptr;
	}

	return pCamera;
}

void CCamera::Free()
{
	CComponent::Free();
}
