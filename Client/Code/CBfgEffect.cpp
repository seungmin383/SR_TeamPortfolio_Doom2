#include "pch.h"	
#include "CBfgEffect.h"

CBfgEffect::CBfgEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEffect(pGraphicDev)
{
}

CBfgEffect::CBfgEffect(const CBfgEffect& rhs)
	: CEffect(rhs)
{
}

CBfgEffect::~CBfgEffect()
{
}

HRESULT CBfgEffect::Ready_GameObject()
{
	CEffect::Ready_GameObject();

	if (FAILED(Add_Component()))
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

_int CBfgEffect::Update_GameObject(const _float& fTimeDelta)
{
	CEffect::Update_GameObject(fTimeDelta);
	Bill_Board();

	if (m_pAnimCom->Get_isAnimEnd())
		return -1;

	return 0;
}

void CBfgEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CEffect::LateUpdate_GameObject(fTimeDelta);

	m_pAnimCom->Play_Animation(0, 0, fTimeDelta);
}

void CBfgEffect::Render_GameObject(const _float& fTimeDelta)
{
	CEffect::Render_GameObject(fTimeDelta);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Set_Texture(0);
	m_pBufferCom->Render_Buffer();
}

void CBfgEffect::Reset_Effect(_vec3 vPos)
{
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pTransformCom->Set_Scale(_vec3{ 2.f, 2.f, 2.f });
	m_pAnimCom->SetFrameDefault();

	m_bDead = FALSE;
}

CEffect* CBfgEffect::Clone()
{
	CEffect* pInstance = new CBfgEffect(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CBfgEffect Clone Failed");
		return nullptr;
	}
	return pInstance;
}

CEffect* CBfgEffect::Clone(_vec3 vPos)
{
	CEffect* pInstance = new CBfgEffect(*this);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CBfgEffect Clone Failed");
		return nullptr;
	}

	//pInstance->Get_Transform()->Set_Pos(vPos.x, vPos.y, vPos.z);

	return pInstance;
}

HRESULT CBfgEffect::Add_Component()
{
	CEffect::Add_Component();

	CComponent* pComponent = nullptr;

	m_pTextureCom = dynamic_cast<Engine::CTexture*>(
		CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_BFGEffect_Texture"));

	if (nullptr == m_pTextureCom)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

	return S_OK;
}

CBfgEffect* CBfgEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBfgEffect* pInstance = new CBfgEffect(pGraphicDev);

	if (pInstance == nullptr)
	{
		Safe_Release(pInstance);
		MSG_BOX("CBfgEffect Create Failed");
		return nullptr;
	}

	return pInstance;
}
void CBfgEffect::Free()
{
	CEffect::Free();
}
