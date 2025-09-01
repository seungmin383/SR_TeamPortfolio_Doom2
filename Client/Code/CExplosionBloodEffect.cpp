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
	CEffect::Ready_GameObject();

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pAnimCom->Add_Animation(
		0,
		m_pTextureCom,
		m_pBufferCom,
		5,
		1,
		0,
		4,
		0.1f,
		false,
		false
	);

	//m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);65

	m_bDead = FALSE;
	return S_OK;
}

_int CExplosionBloodEffect::Update_GameObject(const _float& fTimeDelta)
{
	CEffect::Update_GameObject(fTimeDelta);
	Bill_Board();

	if (m_pAnimCom->Get_isAnimEnd())
		return -1;

	return 0;
}

void CExplosionBloodEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CEffect::LateUpdate_GameObject(fTimeDelta);

	m_pAnimCom->Play_Animation(0, 0, fTimeDelta);
}

void CExplosionBloodEffect::Render_GameObject(const _float& fTimeDelta)
{
	CEffect::Render_GameObject(fTimeDelta);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Set_Texture(m_iCurFrame);
	m_pBufferCom->Render_Buffer();
}

void CExplosionBloodEffect::Reset_Effect(_vec3 vPos)
{
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pTransformCom->Set_Scale(_vec3{ 1.f, 1.f, 1.f });
	m_pAnimCom->SetFrameDefault();
	m_iCurFrame = rand() % 3;
	m_bDead = FALSE;
}

CEffect* CExplosionBloodEffect::Clone()
{
	CEffect* pInstance = new CExplosionBloodEffect(*this);

	if (FAILED(pInstance->Ready_GameObject()))
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

	//pInstance->Get_Transform()->Set_Pos(vPos.x, vPos.y, vPos.z);

	return pInstance;
}

HRESULT CExplosionBloodEffect::Add_Component()
{
	CEffect::Add_Component();

	CComponent* pComponent = nullptr;

	m_pTextureCom = dynamic_cast<Engine::CTexture*>(
		CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_ExplosionBlood_Texture"));

	if (nullptr == m_pTextureCom)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

	return S_OK;
}

CExplosionBloodEffect* CExplosionBloodEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CExplosionBloodEffect* pInstance = new CExplosionBloodEffect(pGraphicDev);

	if (pInstance == nullptr)
	{
		Safe_Release(pInstance);
		MSG_BOX("CExplosionBloodEffect Create Failed");
		return nullptr;
	}

	return pInstance;
}
void CExplosionBloodEffect::Free()
{
	CEffect::Free();
}
