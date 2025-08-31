#include "pch.h"	
#include "CExplosionBloodEffect.h"

CExplosionBloodEffect::CExplosionBloodEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEffect(pGraphicDev)	
{
}

CExplosionBloodEffect::CExplosionBloodEffect(const CExplosionBloodEffect& rhs)
	: CEffect(rhs)
{
}

CExplosionBloodEffect::~CExplosionBloodEffect()
{
}

HRESULT CExplosionBloodEffect::Ready_GameObject()
{
	return S_OK;
}

_int CExplosionBloodEffect::Update_GameObject(const _float& fTimeDelta)
{
	CEffect::Update_GameObject(fTimeDelta);

	return 0;
}

void CExplosionBloodEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CEffect::LateUpdate_GameObject(fTimeDelta);
}

void CExplosionBloodEffect::Render_GameObject(const _float& fTimeDelta)
{
	CEffect::Render_GameObject(fTimeDelta);

}

CEffect* CExplosionBloodEffect::Clone()
{
	CEffect* pInstance = new CExplosionBloodEffect(*this);

	if(FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CExplosionBloodEffect Clone Failed");
		return nullptr;
	}
	return pInstance;
}

CEffect* CExplosionBloodEffect::Clone(_vec3 vPos)
{
	CEffect* pInstance = new CExplosionBloodEffect(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CExplosionBloodEffect Clone Failed");
		return nullptr;
	}

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	return pInstance;
}


HRESULT CExplosionBloodEffect::Add_Component()
{
	CEffect::Add_Component();

	CComponent* pComponent = nullptr;

	m_pTextureCom = dynamic_cast<Engine::CTexture*>(
		CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_ExplosionBloodEffect_Texture"));	
	if (nullptr == m_pTextureCom)
		return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

}

CExplosionBloodEffect* CExplosionBloodEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CExplosionBloodEffect* pInstance = new CExplosionBloodEffect(pGraphicDev);

	return pInstance;
}
void CExplosionBloodEffect::Free()
{
	CEffect::Free();
}
