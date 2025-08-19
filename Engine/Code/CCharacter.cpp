#include "CCharacter.h"

CCharacter::CCharacter(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
	,m_iHp(0) , m_iAtkDmg(0), m_fAtkSpeed(0.f)
{
}

CCharacter::CCharacter(const CCharacter& rhs)
	:CGameObject(rhs)
	, m_iHp(rhs.m_iHp), m_iAtkDmg(rhs.m_iAtkDmg), m_fAtkSpeed(m_fAtkSpeed)
{
}

CCharacter::~CCharacter()
{
}

HRESULT         CCharacter::Ready_GameObject()
{
	if (FAILED(CGameObject::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}
_int					CCharacter::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);
	return 0;
}
void					CCharacter::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}
void					CCharacter::Render_GameObject(const _float& fTimeDelta)
{
	CGameObject::Render_GameObject(fTimeDelta);
}
void CCharacter::Free()
{
	CGameObject::Free();
}
