#include "CTransform.h"

CTransform::CTransform()
	: m_vScale(1.f, 1.f, 1.f),
	m_vAngle(0.f, 0.f, 0.f),
	m_bOnTarget(false)
{
	ZeroMemory(m_vInfo, sizeof(m_vInfo));
	D3DXMatrixIdentity(&m_matWorld);
}

CTransform::CTransform(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev), m_vScale(1.f, 1.f, 1.f), m_vAngle(0.f, 0.f, 0.f),
	m_bOnTarget(false)
{
	ZeroMemory(m_vInfo, sizeof(m_vInfo));
	D3DXMatrixIdentity(&m_matWorld);
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs), m_vScale(rhs.m_vScale), m_vAngle(rhs.m_vAngle),
	m_bOnTarget(false)
{
	for (_uint i = 0; i < INFO_END; i++)
		m_vInfo[i] = rhs.m_vInfo[i];
}

CTransform::~CTransform()
{
}

void CTransform::Set_AxisAngle(const _float& fTimeDelta)
{
	//D3DXMatrixIdentity(&m_matRotAxis);

	_vec3 vPos = m_vInfo[INFO_POS];
	_vec3 vDiff = m_vTargetPos - vPos;
	_vec3 vDiffNormal;
	D3DXVec3Normalize(&vDiffNormal, &vDiff);

	_vec3 vAxis;
	D3DXVec3Cross(&vAxis, &vDiffNormal, &m_vInfo[INFO_LOOK]);

	_float deg = D3DXVec3Dot(&vDiffNormal, &m_vInfo[INFO_LOOK]);

	D3DXMatrixRotationAxis(&m_matRotAxis, &vAxis, deg);

	m_matWorld *= m_matRotAxis;
}

HRESULT CTransform::Ready_Transform()
{
	D3DXMatrixIdentity(&m_matWorld);

	for (_uint i = 0; i < INFO_END; i++)
		memcpy(&m_vInfo[i], &m_matWorld.m[i][0], sizeof(_vec3));

	return S_OK;
}

_int CTransform::Update_Component(const _float& fTimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);

	for (_uint i = 0; i < INFO_POS; i++)
		memcpy(&m_vInfo[i], &m_matWorld.m[i][0], sizeof(_vec3));

	//크기 변환
	for (_uint i = 0; i < INFO_POS; i++)
	{
		D3DXVec3Normalize(&m_vInfo[i], &m_vInfo[i]);
		m_vInfo[i] *= *(((float*)&m_vScale) + i);
	}

	//회전 변환
	_matrix		matRot[ROT_END];
	D3DXMatrixRotationX(&matRot[ROT_X], m_vAngle.x);
	D3DXMatrixRotationY(&matRot[ROT_Y], m_vAngle.y);
	D3DXMatrixRotationZ(&matRot[ROT_Z], m_vAngle.z);

	for (_uint i = 0; i < INFO_POS; i++)
	{
		for (_uint j = 0; j < ROT_END; j++)
		{
			D3DXVec3TransformNormal(&m_vInfo[i], &m_vInfo[i], &matRot[j]);
		}
	}

	///
	D3DXMatrixIdentity(&m_matRotAxis);
	if (m_bOnTarget)
	{
		//Set_AxisAngle(fTimeDelta);
		_vec3 vPos = m_vInfo[INFO_POS];
		_vec3 vDiff = m_vTargetPos - vPos;
		_vec3 vDiffNormal;
		D3DXVec3Normalize(&vDiffNormal, &vDiff);

		_vec3 vAxis;
		D3DXVec3Cross(&vAxis, &vDiffNormal, &m_vInfo[INFO_LOOK]);

		_float deg = D3DXVec3Dot(&vDiffNormal, &m_vInfo[INFO_LOOK]);

		D3DXMatrixRotationAxis(&m_matRotAxis, &vAxis, acosf(deg));

		for (_uint i = 0; i < INFO_POS; i++)
		{
			D3DXVec3TransformNormal(&m_vInfo[i], &m_vInfo[i], &m_matRotAxis);
		}
	}
	///

	for (_uint i = 0; i < INFO_END; i++)
	{
		memcpy(&m_matWorld.m[i][0], &m_vInfo[i], sizeof(_vec3));
	}

	return 0;
}

void CTransform::LateUpdate_Component()
{
}

CComponent* CTransform::Clone()
{
	return new CTransform(*this);
}

CTransform* CTransform::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTransform* pTransform = new CTransform(pGraphicDev);

	if (FAILED(pTransform->Ready_Transform()))
	{
		Safe_Release(pTransform);
		MSG_BOX("Transform Create Failed");
		return nullptr;
	}

	return pTransform;
}

void CTransform::Free()
{
	CComponent::Free();
}