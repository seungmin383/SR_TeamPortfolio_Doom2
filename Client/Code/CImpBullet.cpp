#include "pch.h"
#include "CImpBullet.h"

CImpBullet::CImpBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	:CBullet(pGraphicDev)
{
}

CImpBullet::CImpBullet(const CImpBullet& rhs)
	:CBullet(rhs)
{
}

CImpBullet::~CImpBullet()
{
}

HRESULT CImpBullet::Ready_GameObject()
{
	return E_NOTIMPL;
}

_int CImpBullet::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

void CImpBullet::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CImpBullet::Render_GameObject(const _float& fTimeDelta)
{
}

HRESULT CImpBullet::Add_Component()
{
	CBullet::Add_Component();

	CComponent* pComponent = NULL;
	//texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Imp_Bullet_Texture"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });
}

CImpBullet* CImpBullet::Clone()
{
	CImpBullet* pBullet = new CImpBullet(*this);
	if (pBullet == nullptr)
		return nullptr;

	if (FAILED(pBullet->Ready_GameObject()))
	{
		Safe_Release(pBullet);
		return	nullptr;
	}

	return pBullet;
}

void CImpBullet::Free()
{
	CBullet::Free();
}
