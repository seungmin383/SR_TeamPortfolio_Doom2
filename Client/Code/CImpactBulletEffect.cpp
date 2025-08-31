#include "pch.h"	
#include "CImpactBulletEffect.h"

CImpactBulletEffect::CImpactBulletEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEffect(pGraphicDev)
{
}

CImpactBulletEffect::CImpactBulletEffect(const CImpactBulletEffect& rhs)
	: CEffect(rhs)
{
}

CImpactBulletEffect::~CImpactBulletEffect()
{
}

HRESULT CImpactBulletEffect::Ready_GameObject()
{
	CEffect::Ready_GameObject();

	if(FAILED(Add_Component()))
		return E_FAIL;

	m_pAnimCom->Add_Animation(
		0,
		m_pTextureCom,
		m_pBufferCom,
		6,
		1,
		0,
		5,
		0.15f,
		false,
		false
	);

	//m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);65

	m_bDead = FALSE;
	return S_OK;
}

_int CImpactBulletEffect::Update_GameObject(const _float& fTimeDelta)
{                                                                    
	CEffect::Update_GameObject(fTimeDelta);
	Bill_Board();

	if (m_pAnimCom->Get_isAnimEnd())
		return -1;

	return 0;
}

void CImpactBulletEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CEffect::LateUpdate_GameObject(fTimeDelta);

	m_pAnimCom->Play_Animation(0, 0, fTimeDelta);
}

void CImpactBulletEffect::Render_GameObject(const _float& fTimeDelta)
{
	CEffect::Render_GameObject(fTimeDelta);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Set_Texture(0);
	m_pBufferCom->Render_Buffer();
}

void CImpactBulletEffect::Reset_Effect(_vec3 vPos)
{
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pTransformCom->Set_Scale(_vec3{ 1.f, 1.f, 1.f });
	m_pAnimCom->SetFrameDefault();
		
	m_bDead = FALSE;
}

CEffect* CImpactBulletEffect::Clone()
{
	CEffect* pInstance = new CImpactBulletEffect(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CImpactBulletEffect Clone Failed");
		return nullptr;
	}
	return pInstance;
}

CEffect* CImpactBulletEffect::Clone(_vec3 vPos)
{
	CEffect* pInstance = new CImpactBulletEffect(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CImpactBulletEffect Clone Failed");
		return nullptr;
	}

	//pInstance->Get_Transform()->Set_Pos(vPos.x, vPos.y, vPos.z);

	return pInstance;
}

HRESULT CImpactBulletEffect::Add_Component()
{
	CEffect::Add_Component();

	CComponent* pComponent = nullptr;

	m_pTextureCom = dynamic_cast<Engine::CTexture*>(
		CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_ImpactBulletEffect_Texture"));

	if (nullptr == m_pTextureCom)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

	return S_OK;
}

CImpactBulletEffect* CImpactBulletEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CImpactBulletEffect* pInstance = new CImpactBulletEffect(pGraphicDev);

	if (pInstance == nullptr)
	{
		Safe_Release(pInstance);	
		MSG_BOX("CImpactBulletEffect Create Failed");
		return nullptr;
	}

	return pInstance;
}
void CImpactBulletEffect::Free()
{
	CEffect::Free();
}
