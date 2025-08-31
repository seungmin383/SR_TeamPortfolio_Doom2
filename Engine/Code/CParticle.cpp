#include "CParticle.h"


CParticle::CParticle(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CParticle::CParticle(const CParticle& rhs)
	: CGameObject(rhs)
{
}

CParticle::~CParticle()
{
}

HRESULT CParticle::Ready_GameObject()
{
	if (FAILED(Add_Component()))
	{
		return E_FAIL;
	}

	return S_OK;
}

_int CParticle::Update_GameObject(const _float& fTimeDelta)
{
	return 0;
}

void CParticle::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CParticle::Render_GameObject(const _float& fTimeDelta)
{

}

CGameObject* CParticle::Clone()
{
	CParticle* pInstance = new CParticle(*this);

	if(FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CParticle Clone Failed");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CParticle::Clone(CGameObject* _Owner)
{
	return nullptr;
}

HRESULT CParticle::Add_Component()
{
	return S_OK;
}

CParticle* CParticle::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CParticle* pInstance = new CParticle(pGraphicDev);

    return pInstance;
}

void CParticle::Free()
{
	CGameObject::Free();
}
